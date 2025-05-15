<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : 093_ini.inc                                                  //
// Version     : 24.1                                                         //
// Begin       : 2022-03-07                                                   //
// Last Change : 2024-03-14                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * PF 93: Check for Updates
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$logo=fxGetLogo('big');
$cvd=$GLOBALS['fxpglobals']['dbparam']['versionscode_db'];
$ivp=(int)$GLOBALS['fxpglobals']['dbparam']['version_patch'];
$cvp=substr('00'.$ivp,-3);

echo('<table border="0" cellspacing="0" cellpadding="0">');
echo('	<tr>');
echo('		<td valign="top"><div style="position:relative;"><div style="position:absolute;left:13px;top:12px;width:504px;height:504px;background:#fff;border-radius:50%;box-shadow:4px 4px 8px rgba(0,0,0, 0.5);"></div><img src="'.$logo.'" style="position:relative;left:0;top:0;"></div></td>');
echo('		<td width="5%">&nbsp;</td>');
echo('		<td width="95%" valign="top">');

echo('			<table width="100%" border="0" cellspacing="0" cellpadding="16">');

// Current version + patch
$pla=explode('|', meldung(3382));	// Version|No Version|Patch|No Patch
if(function_exists('fxsppSetVersion'))
	fxsppSetVersion();
echo('				<tr>');
echo('					<td valign="top">');
echo('						<h1 class="tshadow">fx-project '.$v_type.' Edition V'.$v_fxp.'</h1>');
if($ivp)	// Patch
	echo('						'.$pla[2].' '.$ivp.' <i>('.get_entry($GLOBALS['fxpglobals']['dbparam']['date_patch'], 'datum').')</i>');
else		// No Patch
	echo('						<i class="grey">('.$pla[3].')</i>');
echo('					</td>');
echo('				</tr>');

// Checking fx-project OS webpage $var1 for latest version
$url='https://www.fx-project.org';
echo('				<tr>');
echo('					<td valign="top">');
echo('						<hr class="fxhr" style="border-top:1px solid #dddddd;"><br /><br /><br />'.$GLOBALS['nl']);
echo('						'.meldung(3383, true, '<b>'.$url.'</b>').'... ');
$ca=getPageCURL($url.'/xvers.php');
//fxDebug($ca,'$ca', 0);
// ...Error
if(!fxIsArray($ca) || !isset($ca['error']) || $ca['error'] || !isset($ca['content']) || (strlen((string)$ca['content']) != 13))
{
	$ema=explode('|', meldung(3384));	// ERROR|The webpage returned the following error message|The webpage did not return any error messages or a valid version string, this usually means|The webpage could not be contacted! (Maybe your internet is down or your firewall does not allow access)|Try Again
	// ERROR
	echo('<div style="display:inline;padding:2px 8px;color:#e1001a;font-weight:bolder;border:1px solid #e1001a;border-radius:16px;box-shadow:2px 2px 3px rgba(225,0,26, 0.5) inset;">'.$ema[0].'</div><br /><br /><br />'.$GLOBALS['nl']);
	if(fxIsArray($ca) && isset($ca['error']) && strlen((string)$ca['error']))
	{
		// The webpage returned the following error message
		echo('<span class="grey">&rarr; '.$ema[1].':</span><br /><br />'.$GLOBALS['nl']);
		$msg=$ca['error'];
	}
	else
	{
		// The webpage did not return any error messages or a valid version string, this usually means
		echo('<span class="grey">&rarr; '.$ema[2].':</span><br /><br />'.$GLOBALS['nl']);
		// The webpage could not be contacted! (Maybe your internet is down or your firewall does not allow access)
		$msg=$ema[3];
	}
	echo('<div class="s4 bold red bshadow" style="margin-left:18px;padding:16px;border:2px solid #e1001a;">'.$msg.'</div>'.$GLOBALS['nl']);
	// Button: "Try Again"
	echo('<br /><br /><br /><br /><br />'.fieldSubmit('retry',$ema[4]).$GLOBALS['nl']);
}
// ...Success
else
{
	$sma=explode('|', meldung(3385));	// SUCCESS|The returned latest version status is|Congratulations! You already have the latest available version.|A new fx-project OS version is available.|A new patch for your current fx-project OS version is available.|Download
	// SUCCESS
	echo('<div style="display:inline;padding:2px 8px;color:#009f6b;font-weight:bolder;border:1px solid #009f6b;border-radius:16px;box-shadow:2px 2px 3px rgba(0,159,107, 0.5) inset;">'.$sma[0].'</div><br /><br /><br />'.$GLOBALS['nl']);
	// The returned latest version status is
	echo('<span class="grey">&rarr; '.$sma[1].':</span><br /><br />'.$GLOBALS['nl']);
	$lvd=substr((string)$ca['content'],0,9);
	$lvp=substr((string)$ca['content'],-3);
//echo('cvd: [<b>'.$cvd.'</b>] - lvd: [<b>'.$lvd.'</b>]<br />cvp: [<b>'.$cvp.'</b>] - lvp: [<b>'.$lvp.'</b>]<br />');
	// Congratulations! You already have the latest available version.
	if(($cvd > $lvd) || (($cvd == $lvd) && ($cvp >= $lvp)))
		echo('<div class="s4 bold green bshadow" style="margin-left:18px;padding:16px;border:2px solid #009f6b;">'.$sma[2].'</div>'.$GLOBALS['nl']);
	else
	{
		// A new fx-project OS version is available.
		if($cvd < $lvd)
		{
			$dt=$pla[0];	// Version
			$dvd=(int)substr((string)$lvd,0,3).'.'.(int)substr((string)$lvd,3,3).'.'.(int)substr((string)$lvd,6);
			while(substr((string)$dvd,-2) == '.0')
				$dvd=substr((string)$dvd,0,-2);
			$msg=$sma[3].'<br /><br />&rarr; '.$dt.' '.$dvd;
		}
		// A new patch for your current fx-project OS version is available.
		else
		{
			$dt=$pla[2];	// Patch
			$dvp=(int)$lvp;
			$msg=$sma[4].'<br /><br />&rarr; '.$dt.' #'.$dvp;
		}
		echo('<div class="s4 bold blue bshadow" style="margin-left:18px;padding:16px;border:2px solid #006b9f;">'.$msg.'</div>'.$GLOBALS['nl']);
		// Button: "Download"
		$lnk=$url.'/';
		if($GLOBALS['fxpglobals']['lang'] == 1)	// German?
			$lnk .= 'de';
		else
			$lnk .= 'en';
		$lnk .= '/downloads.html';
		echo('<br /><br /><br /><br /><br /><a href="'.$lnk.'" target="_blank"><span class="btn">'.$sma[5].' '.$dt.'</span></a>'.$GLOBALS['nl']);
	}
}
echo('					</td>');
echo('				</tr>');

echo('			</table>');
echo('		</td>');
echo('	</tr>');
echo('</table>');
?>