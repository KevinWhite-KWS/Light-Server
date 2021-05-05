<#
    Randomly uploads programs every 5 - 60 seconds to the light server.
    This demostrates how the light server can be easily programmed
    and also provides a mechanism for testing stability.

    Usage:
        1. You can change the colours that are used to generate by modifying
           the variable $coloursToUse/

    Kevin White
    13 April 2021
#>

Clear-Host


. "$PSScriptRoot\LDL-Server-API.ps1"

# add the colours you wish to see in the generated effects here
$coloursToUse = [Colour[]](
    [Colour]::new(255,0,0), 
    [Colour]::new(0,255,0), 
    [Colour]::new(0,0,255)
)

$discoverService=[LdlServerDiscover]::new("8888")

# do the initial server discovery
$discoveredServers = @()
$apiProtocol= "http"
$accessToken = "U3VwZXI6MXhZYTFtYW4yKg=="
$header = @{
    "Authorization"="Basic " + $accessToken
    "Content-Type"="application/json; charset=utf-8"
}



do {
    $currentProgram = ""

    $ldlProgram = [LDLProgram]::new("Random program")
    $infiniteLoop = $ldlProgram.AddInstruction([LoopLDLInstruction]::new(0))

    # add between 5 - 15 randomly generated instructions that
    # use any of the colours in $coloursToUse
    $numRandomInstructions = [LDLHelper]::RandomNumber(5, 15)
    for($i = 0; $i -lt $numRandomInstructions; $i++) {
        $randomIns = [LDLHelper]::RandomNumber(0, 7)
        $randomDuration = [LDLHelper]::RandomNumber(1, 3)

        switch($randomIns) {
            (0) {   # Clear
                $ins = $infiniteLoop.AddInstruction([ClearEffectLDLInstruction]::new($randomDuration))
            }
            (1) {   # Solid
                $ins = $infiniteLoop.AddInstruction([SolidEffectLDLInstruction]::new($randomDuration, [LDLHelper]::RandomColour($coloursToUse)))
            }
            (2) {   # Pattern
                $randomColourQuantities = [LDLHelper]::RandomColourQuantities($coloursToUse, 2, 5, 2, 5)
                $ins = [PatternEffectLDLInstruction]::new($randomDuration, $randomColourQuantities)
                $ins = $infiniteLoop.AddInstruction($ins)
            }
            (3) {   # Slider
                $ins = [SliderEffectLDLInstruction]::new($randomDuration, 
                    [LDLHelper]::RandomNumber(1, 5),
                    [LDLHelper]::RandomBool(),
                    [LDLHelper]::RandomNumber(0, 100),
                    [LDLHelper]::RandomNumber(0, 100),
                    [LDLHelper]::RandomColour($coloursToUse),
                    [LDLHelper]::RandomColour($coloursToUse)
                    )
                $ins = $infiniteLoop.AddInstruction($ins)
            }
            (4) {   # Fade
                $ins = [FadeEffectLDLInstruction]::new($randomDuration,
                    [LDLHelper]::RandomNumber(1, 10),
                    [LDLHelper]::RandomBool(),
                    [LDLHelper]::RandomColour($coloursToUse),
                    [LDLHelper]::RandomColour($coloursToUse)
                )
                $ins = $infiniteLoop.AddInstruction($ins)
            }
            (5) {   # Stochastic
                $ins = [StochasticEffectLDLInstruction]::new($randomDuration, [LDLHelper]::RandomColours($coloursToUse, 2, 5))
                $ins = $infiniteLoop.AddInstruction($ins)
            }
            (6) {   # Blocks
                [byte]$randomNumberOfBlocks = [LDLHelper]::RandomNumber(2, 10)
                [byte]$blockWidthPercent = 100 / $randomNumberOfBlocks
                $colourQuantities = [Collections.Generic.List[ColourQuantity]]::new()
                [byte]$blockWidthsTotal = 0
                for($blockCounter = $randomNumberOfBlocks - 1; $blockCounter -gt -1; $blockCounter--) {
                    $percentWidth = If ($blockCounter -eq 0) { 100 - $blockWidthsTotal} else { $blockWidthPercent}
                    $blockWidthsTotal += $percentWidth
                    $colourQuantities.Add([ColourQuantity]::new([LDLHelper]::RandomColour($coloursToUse), $percentWidth))
                }
                $ins = [BlocksEffectLDLInstruction]::new($randomDuration, $colourQuantities)
                $ins = $infiniteLoop.AddInstruction($ins)
            }
            (7) {   # Rainbow
                $ins = [RainbowEffectLDLInstruction]::new($randomDuration,
                    [LDLHelper]::RandomNumber(5, 60),
                    [LDLHelper]::RandomNumber(5, 60),
                    [LDLHelper]::RandomBool(),
                    [LDLHelper]::RandomColours($coloursToUse, 2, 5)
                )
                $ins = $infiniteLoop.AddInstruction($ins)
            }
        }
    }

    $currentProgram = $ldlProgram.SerializeToLDL()

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
                Write-Host $currentProgram
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