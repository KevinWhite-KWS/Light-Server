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

# $morseMessage = "sos"
# $morseMessage = "hello world"
$morseMessage = "merry christmas"
$ldlServerIp = ""
$dotRenderingFrameDuration = 2
$dotDashColour = [Colour]::new(255,0,0)

# generate the morse code
# see: https://en.wikipedia.org/wiki/Morse_code

$ldlProgram = [LDLProgram]::new("Morse code program")

$morseCode = @{
    "a"=".=";
    "b"="=...";
    "c"="=.=.";
    "d"="=..";
    "e"=".";
    "f"="..=.";
    "g"="==.";
    "h"="....";
    "i"="..";
    "j"=".===";
    "k"="=.=";
    "l"=".=..";
    "m"="==";
    "n"="=.";
    "o"="===";
    "p"=".==.";
    "q"="==.=";
    "r"=".=.";
    "s"="...";
    "t"="=";
    "u"="..=";
    "v"="...=";
    "w"=".==";
    "x"="=..=";
    "y"="=.==";
    "z"="==..";
    "1"=".====";
    "2"="..===";
    "3"="...==";
    "4"="....=";
    "5"=".....";
    "6"="=....";
    "7"="==...";
    "8"="===..";
    "9"="====.";
    "0"="=====";
}

$morseMessage = $morseMessage.ToLower()
for($i = 0; $i -le $morseMessage.Length - 1; $i++) {
    $morseChar = $morseMessage[$i]
    Write-Host $morseChar

    if($morseChar -eq " ") {
        # space between words (7 delay length)
        $ldlProgram.AddInstruction([ClearEffectLDLInstruction]::new(7 * $dotRenderingFrameDuration))
    } else {
        # space between letters (3 delay length)
        $ldlProgram.AddInstruction([ClearEffectLDLInstruction]::new(3 * $dotRenderingFrameDuration))

        if($morseCode.ContainsKey([string]$morseChar)) {
            $morseEncodingOfChar = $morseCode[[string]$morseChar]
            for($j = 0; $j -lt $morseEncodingOfChar.Length; $j++) {
                $morseEncoding = $morseEncodingOfChar[$j];

                # on length is 1 unit for . and 3 units for = (dash)
                $onLength = if($morseEncoding -eq ".") { 1 } else {3}
                $ins = $ldlProgram.AddInstruction([SolidEffectLDLInstruction]::new($onLength * $dotRenderingFrameDuration, $dotDashColour))

                if($j -ne $morseEncodingOfChar.Length - 1) {
                    # space after a single letter
                    $ins = $ldlProgram.AddInstruction([ClearEffectLDLInstruction]::new($dotRenderingFrameDuration))
                }
            }
        }
    }
}

# add a final clear
$ins = $ldlProgram.AddInstruction([ClearEffectLDLInstruction]::new($dotRenderingFrameDuration))

# send the program to all LDL servers
[LDLHelper]::UploadLDLProgramVerbose($ldlProgram, "192.168.6.77", 8888)