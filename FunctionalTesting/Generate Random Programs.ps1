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
$discoveredServers = @()

Function GenerateRandomNumberInHex($min = 1, $max = 10, $digits = 2) {
    $randomValue = Get-Random -Minimum $min -Maximum $max
    return "{0:X$digits}" -f $randomValue
}

Function GetRandomColour([string[]]$possibleColours) {
    $randomColourIndex = Get-Random -Minimum 0 -Maximum $possibleColours.Count
    return $possibleColours[$randomColourIndex]
}

Function GenerateClearInstruction() {
    $renderingFrameDuration = GenerateRandomNumberInHex
    return """00${renderingFrameDuration}0000"""
}

Function GenerateSolidInstruction([string[]]$possibleColours) {
    $randomColour = GetRandomColour -possibleColours $possibleColours
    $renderingFrameDuration = GenerateRandomNumberInHex
    return """01${renderingFrameduration}0000${randomColour}"""
}

Function GeneratePatternInstruction([string[]]$possibleColours) {
    $renderingFrameDuration = GenerateRandomNumberInHex
    $ins = "02${renderingFrameDuration}0000"
    $widths = ""
    $colours = ""
    $numberOfElementsInPattern = Get-Random -Minimum 1 -Maximum 5
    $numberOfElementsHex = "{0:X2}" -f $numberOfElementsInPattern
    for($i = 0; $i -lt $numberOfElementsInPattern; $i++) {
        $colourWidth = Get-Random -Minimum 1 -Maximum 4
        $colourWidthHex = "{0:X2}" -f $colourWidth
        $colour = GetRandomColour -possibleColours $possibleColours
        $widths += $colourWidthHex
        $colours += $colour
    }
    $ins += $numberOfElementsHex + $widths + $colours
    return """$ins"""
}

Function GenerateSliderInstruction([string[]]$possibleColours) {
    $renderingFrameDuration = GenerateRandomNumberInHex -min 1 -max 5
    $ins = "03${renderingFrameDuration}0000"
    $sliderWidth = GenerateRandomNumberInHex -min 1 -max 10
    $startNear = GenerateRandomNumberInHex -min 0 -max 1 -digits 1
    $headLength = GenerateRandomNumberInHex -min 0 -max 100
    $tailLength = GenerateRandomNumberInHex -min 0 -max 100
    $sliderColour = GetRandomColour -possibleColours $possibleColours
    $backgroundColour = GetRandomColour -possibleColours $possibleColours
    $ins = """${ins}${sliderWidth}${startNear}${headLength}${tailLength}${sliderColour}${backgroundColour}"""
    return $ins
}

Function GenerateFadeInstruction([string[]]$possibleColours) {
    $renderingFrameDuration = GenerateRandomNumberInHex -min 1 -max 5
    $ins = "04${renderingFrameDuration}0000"    
    $step = GenerateRandomNumberInHex -min 1 -max 50
    $fadeIn = GenerateRandomNumberInHex -min 0 -max 1
    $startColour = GetRandomColour -possibleColours $possibleColours
    $endColour = GetRandomColour -possibleColours $possibleColours
    $ins = """${ins}${step}${fadeIn}${startColour}${endColour}"""
    return $ins
}

Function GenerateStochasticInstruction([string[]]$possibleColours) {
    $renderingFrameDuration = GenerateRandomNumberInHex
    $ins = "05${renderingFrameDuration}0000"
    $numberOfColours = Get-Random -Minimum 2 -Maximum 50
    $numberOfColoursHex = "{0:X2}" -f $numberOfColours
    $ins += $numberOfColoursHex
    for($i = 0; $i -lt $numberOfColours; $i++) {
        $colour = GetRandomColour -possibleColours $possibleColours
        $ins += $colour
    }
    return """$ins"""
}

Function GenerateBlocksInstruction([string[]]$possibleColours) {
    $renderingFrameDuration = GenerateRandomNumberInHex
    $ins = "06${renderingFrameDuration}0000"
    # for now this instruction is fixed to 4 blocks of 25% each
    $ins += "0419191919"
    for($i = 0; $i -lt 4; $i++) {
        $colour = GetRandomColour -possibleColours $possibleColours
        $ins += $colour
    }
    return """$ins"""
}

Function GenerateRainbowInstruction([string[]]$possibleColours) {
    $renderingFrameDuration = GenerateRandomNumberInHex -min 1 -max 3
    $ins = "07${renderingFrameDuration}0000"
    $length = GenerateRandomNumberInHex -min 1 -max 255
    $steps = GenerateRandomNumberInHex -min 1 -max 255
    $startNear = GenerateRandomNumberInHex -min 0 -max 1 -digits 1
    $ins += $length + $steps + $startNear

    $numberOfColours = Get-Random -Minimum 1 -Maximum 10
    $numberOfColoursHex = "{0:X2}" -f $numberOfColours
    $ins += $numberOfColoursHex
    for($i = 0; $i -lt $numberOfColours; $i++) {
        $colour = GetRandomColour -possibleColours $possibleColours
        $ins += $colour
    }

    return """$ins"""
}


$apiProtocol= "http"
$accessToken = "U3VwZXI6MXhZYTFtYW4yKg=="
$header = @{
    "Authorization"="Basic " + $accessToken
    "Content-Type"="application/json; charset=utf-8"
}
$cr = "`n"
$tab = "`t"
$colours = @("FF0000", "00FF00", "0000FF", "000000")


do {
    # generate a random program here - random number of instructions from 5 to 20
    $programInstructions = New-Object 'Collections.Generic.List[string]'
    $numberOfProgramIns = Get-Random -Minimum 5 -Maximum 10
    for($i = 0; $i -lt $numberOfProgramIns; $i++) {
        $programInsIndex = Get-Random -Minimum 0 -Maximum 8
        $ins = ""
        switch($programInsIndex) {
            (0) { $ins = GenerateClearInstruction }
            (1) { $ins = GenerateSolidInstruction -possibleColours $colours }
            (2) { $ins = GeneratePatternInstruction -possibleColours $colours }
            (3) { $ins = GenerateSliderInstruction -possibleColours $colours }
            (4) { $ins = GenerateFadeInstruction -possibleColours $colours }
            (5) { $ins = GenerateStochasticInstruction -possibleColours $colours }
            (6) { $ins = GenerateBlocksInstruction -possibleColours $colours }
            (7) { $ins = GenerateRainbowInstruction -possibleColours $colours }
        }
        $programInstructions.Add($ins)
    }

    $randomDelay = Get-Random -Minimum 5 -Maximum 60

    # try and discover LDL servers on the local network is necessary
    if($null -eq $discoveredServers -or $discoveredServers.Count -eq 0) {
        # $discoveredServers = DiscoverLdlServers
        $discoveredServers = [LdlServerDiscover]::DiscoverServersVerbose($discoverService)
    }

    try {
        if($null -ne $discoveredServers -and $discoveredServers.Count -gt 0) {
            $discoveredServers | ForEach-Object {
                [LdlServerInfo]$ldlServerInfo = $_
                $serverIp = $ldlServerInfo.ip
                $serviceUrl = "${apiProtocol}://$serverIp/program"
                # Write-Host "Sending $currentProgramName for $randomDelay seconds to server $serverIp : " -ForegroundColor Green -NoNewline
                
                $programIns = ""
                $countProgramIns = 0
                $maxProgramIns = $programInstructions.Count
                $programInstructions | ForEach-Object {
                    $ins = $_
                    $programIns += "$tab$tab$tab$tab${ins}"
                    if($countProgramIns -lt $maxProgramIns - 1) {
                        $programIns += ","
                    }
                    $programIns += "$cr"
                    $countProgramIns++
                };

                $currentProgram = 
                "{$cr" +
                    "$tab""name"" : ""Random"",$cr" +
                    "$tab""instructions"" : [$cr" +
                        "$tab$tab{""repeat"" : {$cr" +
                            "$tab$tab$tab""times"" : 0,$cr" +
                            "$tab$tab$tab""instructions"" : [$cr" +
                                $programIns +
                            "$tab$tab$tab]$cr" +
                        "$tab$tab}}$cr"+
                "$tab]$cr" +
                '}'                

                Write-Host "Sending the following randomly generated program:" -ForegroundColor Green
                Write-Host $currentProgram -ForegroundColor Green
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