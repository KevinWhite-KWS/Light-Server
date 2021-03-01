<#
    Randomly uploads programs every 5 - 60 seconds to the light server.
    The purpose is to test stability.

    Kevin White
    25 Feb 2021
#>

$importFolder = "$PSscriptRoot\Programs\*.ldl"
$apiBaseUrl="http://192.168.5.210"  
$accessToken = "U3VwZXI6MXhZYTFtYW4yKg=="
$header = @{
    "Authorization"="Basic " + $accessToken
    "Content-Type"="application/json; charset=utf-8"
}

$inputPrograms = Get-ChildItem -Path $importFolder

Clear-Host

do {
    $randomProgramIndex = Get-Random -Minimum -0 -Maximum $inputPrograms.Count
    $currentProgram = Get-Content -Path $inputPrograms[$randomProgramIndex]
    $currentProgramName = $inputPrograms[$randomProgramIndex].Name
    $randomDelay = Get-Random -Minimum 5 -Maximum 60

    try {
        $request = Invoke-WebRequest "$apiBaseURl/program" -Body  $currentProgram -Method 'POST' -Headers $header -ContentType "application/json; charset=utf-8"
        Write-Host "Sending $currentProgramName for $randomDelay" -ForegroundColor Green -NoNewline
    }
    catch [Exception] {
        Write-Host "Exception: $_" -ForegroundColor Red
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