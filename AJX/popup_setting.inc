<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : popup_set.inc                                                //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-09-22                                                   //
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
 * Toolbar: "Setting"
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

$tmode=$GLOBALS['fxpglobals']['tr'];
$lang=$GLOBALS['fxpglobals']['lang'];

// Load setting texts in selected language
if(!fxIsArray($GLOBALS['stla']))
	$GLOBALS['stla']=fxGetLanguageTexts('setting', true);
//fxDebug($GLOBALS['stla'],'$GLOBALS[\'stla\']: fxGetLanguageTexts(\'setting\')',0);

// Set headline
if($tmode == 189)
	$headline=$GLOBALS['stla']['hl_matrix'];	// Matrix Settings
else
	$headline=$GLOBALS['stla']['hl_global'];	// Global Settings

// Set switch texts
$yes=$GLOBALS['stla']['sw_yes'];	// Yes
$no=$GLOBALS['stla']['sw_no'];		// No
$on=$GLOBALS['stla']['sw_on'];		// On
$off=$GLOBALS['stla']['sw_off'];	// Off

// Popup mask header
$ph=popupMask($headline, '', '', 'dhlppu');
echo($ph);

$yn0='<span class="switchl0">'.$yes.'</span><span class="switchr1">'.$no.'</span>';	// Yes/No
$yn1='<span class="switchl1">'.$yes.'</span><span class="switchr0">'.$no.'</span>';
$oo0='<span class="switchl0">'.$on.'</span><span class="switchr1">'.$off.'</span>';	// On/Off
$oo1='<span class="switchl1">'.$on.'</span><span class="switchr0">'.$off.'</span>';

$idprefixg='set';
$idprefixl='str';

if($tmode == 189)
{
	$rset=false;
	$wlsras0='';
	if(fxIsArray($_GET) && isset($_GET['wlsras']) && ($_GET['wlsras'] === '0'))
	{
		$wlsras0=' checked';
		$rset=true;
	}
	$wlsras1='';
	if(fxIsArray($_GET) && isset($_GET['wlsras']) && ($_GET['wlsras'] === '1'))
	{
		$wlsras1=' checked';
		$rset=true;
	}
	if(!$rset)
		$wlsras0=' checked';
}

$ca=fxf_getClientFilenames();
$atextmenu=$ca['tmenu-alt'];

$sa=array(
	// Common
	$GLOBALS['stla']['s1_header']=>array(
		'help'=>array('+oo',$GLOBALS['stla']['s1_help']),					// Show help tooltips
		'trmark'=>array('+oo',$GLOBALS['stla']['s1_trmark']),				// Highlight the borders of the actual list line in which the mouse cursor is
		'filter'=>array('+oo',$GLOBALS['stla']['s1_filter']),				// Use filter in special project or person select fields
		'ppsppopalways'=>array('+yn',$GLOBALS['stla']['s1_ppsppopalways'])	// Open always pp/sp budget calculation popup
	),
	// Menu
	$GLOBALS['stla']['s2_header']=>array(
		'amenu'=>array('+yn',$GLOBALS['stla']['s2_amenu'])					// Use alternative menu structure
	),
	// Matrix - Gantt
	$GLOBALS['stla']['s3_header']=>array(
		'showlines'=>array('+oo',$GLOBALS['stla']['s3_showlines']),			// Display line numbers in the Project Structure
		'showhr'=>array('+oo',$GLOBALS['stla']['s3_showhr']),				// Display alloted human resources in the Gantt
		'showframes'=>array('+oo',$GLOBALS['stla']['s3_showframes']),		// Display project frames in the Gantt
		'contnumbers'=>array('+yn',$GLOBALS['stla']['s3_contnumbers']),		// Continuous project numbering
		'alignworkday'=>array('+yn',$GLOBALS['stla']['s3_alignworkday'])	// Align start and end dates on workdays in the Gantt and<br />keep the number of wordays for tasks while moving
	),
	// Matrix - Resource Planning
	$GLOBALS['stla']['s4_header']=>array(
		'wlscbl'=>array('-yn',$GLOBALS['stla']['s4_wlscbl']),				// Display total line of all added up workloads
		'wlscba'=>array('-yn',$GLOBALS['stla']['s4_wlscba']),				// Display workloads of <b>active</b> tasks that are not displayed in the Gantt area
		'wlscbp'=>array('-yn',$GLOBALS['stla']['s4_wlscbp']),				// Display workloads of <b>planned</b> tasks that are not displayed in the Gantt area
		'wlscbs'=>array('-yn',$GLOBALS['stla']['s4_wlscbs'])				// Display workloads of active <b>piled up</b> tasks that are not displayed in the Gantt area
	),
);

if(!file_exists($atextmenu))
	unset($sa[$GLOBALS['stla']['s2_header']]);
if(!fxIsArray($_GET) || !isset($_GET['ppsppopalways']))
	unset($sa[$GLOBALS['stla']['s1_header']]['ppsppopalways']);
if(!fxIsArray($_GET) || !isset($_GET['showlines']))
	unset($sa[$GLOBALS['stla']['s3_header']]);
if(!fxIsArray($_GET) || !isset($_GET['wlscbl']))
	unset($sa[$GLOBALS['stla']['s4_header']]);
else
{
	$sa[$GLOBALS['stla']['s4_header']]['wlscbs'][1] .= '&nbsp;&hellip;<br /><br />';
	$sa[$GLOBALS['stla']['s4_header']]['wlscbs'][1] .= '&nbsp;<input id="'.$idprefixl.'_ra_wlsras_wlscbs" name="'.$idprefixl.'_wlsras" type="radio" value="0"'.$wlsras0.'>&nbsp;&hellip;&nbsp;'.$GLOBALS['stla']['s4_wlscbs_r0'].'<br />';	// added up on next workday in the future
	$sa[$GLOBALS['stla']['s4_header']]['wlscbs'][1] .= '&nbsp;<input id="'.$idprefixl.'_ra_wlsras_wlscbs" name="'.$idprefixl.'_wlsras" type="radio" value="1"'.$wlsras1.'>&nbsp;&hellip;&nbsp;'.$GLOBALS['stla']['s4_wlscbs_r1'].'<br />';	// added up on next workday in the future
}


echo('	<div style="position:relative;left:6px;top:0;width:940px;height:660px;overflow:auto;">');
foreach($sa as $head => $da)
{
	echo('		<br /><b>'.$head.'</b><br /><br />');
	echo('		<table class="rtable" align="center" width="880">');

	$sda=sizeof($da);
	$sdc=1;
	foreach($da as $svar => $sarr)
	{
		$pt=substr($sarr[0],0,1);
		if($pt == '-')
			$idprefix=$idprefixl;
		else
			$idprefix=$idprefixg;

		$tt=substr($sarr[0],1);
		$tx=$sarr[1];

		$v=0;
		if(fxIsArray($_GET) && isset($_GET[$svar]) && $_GET[$svar] && ($_GET[$svar] !== 'false'))
			$v=1;

		$add='<span id="'.$idprefix.'_'.$tt.'_'.$svar.'" class="switchadd"></span>';
		if($tt == 'yn')
		{
			if($v)
				$html=$yn1.$add;
			else
				$html=$yn0.$add;
		}
		else
		{
			if($v)
				$html=$oo1.$add;
			else
				$html=$oo0.$add;
		}

		$ls='';
		if($sdc >= $sda)
			$ls='l';

		echo('			<tr><td class="lscolumn'.$ls.'">'.$tx.'</td><td class="rscolumn'.$ls.'" align="center" valign="top"><div id="div_'.$tt.'_'.$svar.'" class="switch">'.$html.'</div></td></tr>');

		$sdc++;
	}

	echo('		</table><br /><br />');
}
echo('	</div>');

echo('</div>');
?>