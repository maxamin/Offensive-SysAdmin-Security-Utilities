function Parse-JWTtoken {
 
    [cmdletbinding()]
    param([Parameter(Mandatory=$true)][string]$token)
 
    if (!$token.Contains(".") -or !$token.StartsWith("eyJ")) { Write-Error "Invalid token" -ErrorAction Stop }

    $tokenheader = $token.Split(".")[0].Replace('-', '+').Replace('_', '/')

    while ($tokenheader.Length % 4) { 
		$tokenheader += "=" 
	}

    [System.Text.Encoding]::ASCII.GetString([system.convert]::FromBase64String($tokenheader)) | ConvertFrom-Json | fl | Out-Default
 
    $tokenPayload = $token.Split(".")[1].Replace('-', '+').Replace('_', '/')
	
    while ($tokenPayload.Length % 4) { 
		$tokenPayload += "=" 
	}

    $tokenByteArray = [System.Convert]::FromBase64String($tokenPayload)
    $tokenArray = [System.Text.Encoding]::ASCII.GetString($tokenByteArray)

    $tokobj = $tokenArray | ConvertFrom-Json
    return $tokobj
}

$body=@{
	"client_id" = "d3590ed6-52b3-4102-aeff-aad2292ab01c"
	"resource" =  "https://graph.windows.net"
}

# Invoke the request to get device and user codes
$authResponse = Invoke-RestMethod -UseBasicParsing -Method Post -Uri "https://login.microsoftonline.com/common/oauth2/devicecode?api-version=1.0" -Body $body
$user_code =    $authResponse.user_code

write-output $authResponse

$continue = $true
$interval = $authResponse.interval
$expires =  $authResponse.expires_in

# Create body for authentication requests
$body=@{
	"client_id" =  "d3590ed6-52b3-4102-aeff-aad2292ab01c"
	"grant_type" = "urn:ietf:params:oauth:grant-type:device_code"
	"code" =       $authResponse.device_code
	"resource" =   "https://graph.windows.net"
}

# Loop while authorisation is pending or until timeout exceeded
while($continue)
{
	Start-Sleep -Seconds $interval
	$total += $interval

	if($total -gt $expires)
	{
		Write-Error "Timeout occurred"
		return
	}
				
	# Try to get the response. Will give 40x while pending so we need to try&catch
	try
	{
		$response = Invoke-RestMethod -UseBasicParsing -Method Post -Uri "https://login.microsoftonline.com/Common/oauth2/token?api-version=1.0 " -Body $body -ErrorAction SilentlyContinue
	}
	catch
	{
		# This is normal flow, always returns 40x unless successful
		$details=$_.ErrorDetails.Message | ConvertFrom-Json
		$continue = $details.error -eq "authorization_pending"
		Write-Host $details.error

		if(!$continue)
		{
			# Not pending so this is a real error
			Write-Error $details.error_description
			return
		}
	}

	# If we got response, all okay!
	if($response)
	{
		write-output $response
		$jwt = $response.access_token
		
		$output = Parse-JWTtoken -token $jwt
		$upn = $output.upn
		write-output $upn
		Write-output "Dumping Users"
		Connect-AzureAD -AadAccessToken $response.access_token -AccountId $upn
		Get-AzureADUser -All $True | Select-Object -Property * | Out-File AD-users.txt

		Write-output "Dumping Groups"
		Get-AzureADGroup -All $True | Select-Object -Property * | Out-File AD-groups.txt
	
		
		Write-output "Dumping Groups Membership"
		foreach($group in Get-AzureADGroup -All $True) {
			$group.DisplayName | Out-File GroupMembership.txt -Append
			Get-AzureADGroupMember -ObjectId $group.ObjectId -All $True | Out-File GroupMembership.txt -Append
		}

		break
	}
}
