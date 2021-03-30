<#
    Randomly uploads programs every 5 - 60 seconds to the light server.
    The purpose is to test stability.

    Kevin White
    25 Feb 2021
#>

Clear-Host

. "$PSScriptRoot\LDL-Server-UDP-Discover.ps1"

$ldlDiscoveryPort = "8888"
$discoverService=[LdlServerDiscover]::new($ldlDiscoveryPort)

# do the initial server discovery
$discoveredServers = @()#$discoverService.DiscoverServers()

$importFolder = "$PSscriptRoot\Programs\*.ldl"
$apiProtocol= "http"
$accessToken = "U3VwZXI6MXhZYTFtYW4yKg=="
$header = @{
    "Authorization"="Basic " + $accessToken
    "Content-Type"="application/json; charset=utf-8"
}

$inputPrograms = Get-ChildItem -Path $importFolder



do {
    $randomProgramIndex = Get-Random -Minimum -0 -Maximum $inputPrograms.Count
    $currentProgram = Get-Content -Path $inputPrograms[$randomProgramIndex]
    $currentProgramName = $inputPrograms[$randomProgramIndex].Name
    $randomDelay = Get-Random -Minimum 5 -Maximum 60

    # try and discover LDL servers on the local network is necessary
    if($null -eq $discoveredServers -or $discoveredServers.Count -eq 0) {
        $discoveredServers = [LdlServerDiscover]::DiscoverServersVerbose($discoverService)
    }

    try {
        if($null -ne $discoveredServers -and $discoveredServers.Count -gt 0) {
            $discoveredServers | ForEach-Object {
                [LdlServerInfo]$ldlServerInfo = $_
                $serverIp = $ldlServerInfo.ip
                $serviceUrl = "${apiProtocol}://$serverIp/program"
                Write-Host "Sending $currentProgramName for $randomDelay seconds to server $serverIp : " -ForegroundColor Green -NoNewline
                $response = Invoke-WebRequest $serviceUrl -Body  $currentProgram -Method 'POST' -Headers $header -ContentType "application/json; charset=utf-8" -TimeoutSec 10
            }
        }
    }
    catch [Exception] {
        Write-Host ""
        Write-Host "Exception: $_" -ForegroundColor Red
        $randomDelay = 0

        # try and discover ldl servers on the next iteration
        $discoveredServers = $null
    }

    # write a status as each delay second ticks away
    For($n = $randomDelay; $n -ne 0; $n--) {
        if($n % 5 -eq 0) {
            Write-Host $n -ForegroundColor Green -NoNewLine
        } else {
            Write-Host "." -ForegroundColor Green -NoNewline
        }
        Start-Sleep -Seconds 1
    }
    Write-Host ""
} while(1 -ne 0)