<#
    .Description
    Stores the information relevant to about an LDL server including:
        - ip address
        - ldl server version
    Kevin White
    22 Mar 2021
#>
class LdlServerInfo {
    [string]$ip
    [string]$server

    hidden LdlServerInfo() {
    }

    <#
        .Description
        Constructs a new LdlServerInfo by transforming a
        plain object to a strongly=typed LdlServerInfo.
        Kevin White
        22 Mar 21
        .Parameter      hashTable   [HashTable]The plain object that will be transformed to a LdlServerInfo instance
        .Parameter      ip          [string]The IP address of the LDL server
    #>    
    static [LdlServerInfo]FromHashTable($hashTable, $ip) {
        if($null -eq $hashTable `
            -or $null -eq $hashTable.server `
            -or $null -eq $ip) {
            return $null
        }

        $serverInfo = New-Object LdlServerInfo
        $serverInfo.server = $hashTable.server
        $serverInfo.ip = $ip

        return $serverInfo
    }
}

<#
    .Description
    Discovers any LDL servers that might be running of the local
    network by broadcasting a UDP packet on a specific port.
    Kevin White
    22 Mar 2021
#>
class LdlServerDiscover {
    [string]$port = "8888"
    hidden [string]$handshake = "LDL-HOLA?"
    hidden $handshakeBytes
    hidden $maxDiscoverSeconds = 10

    <#
        .Description
        Constructor initialises the discovering port number.
        Kevin White
        22 Mar 21
        .Parameter      port    [int]The port number on which to broadcast the discovery packet.
    #> 
    LdlServerDiscover($port = 8888) {
        $this.port = $port

        $asciiEncoding = new-object system.text.asciiencoding
        $this.handshakeBytes = $asciiEncoding.GetBytes($this.handshake)
    }

    <#
        .Description
        Discovers LDL servers running on the local network
        Kevin White
        22 Mar 21
        .Outputs    [array]Collecton of discovered servers as LdlServerInfo instances.
    #>
    [array]DiscoverServers() {
        $receiveEndpoint = New-Object system.net.ipendpoint([system.net.ipaddress]::Any,$this.port)
        $broadcastEndPoint = New-Object system.net.ipendpoint([system.net.ipaddress]::Broadcast,$this.port)
        
        $asciiEncoding = new-object system.text.asciiencoding
        $discoveredLdlServers = New-Object 'Collections.Generic.List[LdlServerInfo]'

        $udpClient = new-Object system.Net.Sockets.Udpclient
        try {
            $udpClient.EnableBroadcast = $true
            $udpClient.Client.Bind($receiveEndpoint)
            $udpClient.Client.ReceiveTimeout = 2000

            # broadcast the handshake message
            [void]$udpClient.Send($this.handshakeBytes,$this.handshakeBytes.length, $broadcastEndPoint)

            # listen for responses for 10 seconds max
            $startTime = $(get-date)
            $elapsed = 0
            $receivebytes = $null
            $countFailures = 0
            $ldlServersPotentiallyFound = @{}
            while($elapsed -le $this.maxDiscoverSeconds -and $countFailures -lt 2) {
                Try {
                    $receivebytes = $udpClient.Receive([ref]$receiveEndpoint)
                    $from = $receiveEndpoint.Address.ToString()

                    [string]$returndata = $asciiEncoding.GetString($receivebytes)

                    if($null -ne $returndata -and $returndata -ne $this.handshake) {
                        # we get one!
                        $ldlServersPotentiallyFound.Add($from, $returndata)
                    }
                } Catch {
                    $countFailures++
                }
                $currentTime = $(get-date)
                $elapsedTime = $currentTime - $startTime
                $elapsed = $elapsedTime.TotalSeconds
            }

            # see what we got - any LDL servers?
            
            $ldlServersPotentiallyFound.Keys | ForEach-Object {
                $ip = $_
                $reply = $ldlServersPotentiallyFound[$ip]

                $replyObj = $reply | ConvertFrom-Json

                if($null -ne $replyObj) {
                    [LdlServerInfo]$serverInfo = [LdlServerInfo]::FromHashTable($replyObj, $ip)
                    if($null -ne $serverInfo) {
                        $discoveredLdlServers.Add($serverInfo)
                    }
                }
            };
        }
        finally {
            $udpClient.Close()
        }

        return $discoveredLdlServers
    }
}

# example simple usage
# [LdlServerDiscover]$discoverService = New-Object LdlServerDiscover -ArgumentList "8888"
# $discoveredServers = $discoverService.DiscoverServers()
