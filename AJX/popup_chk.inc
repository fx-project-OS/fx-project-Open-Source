<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : popup_chk.inc                                                //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-02-12                                                   //
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
 * Mask: "Set Checkbox"
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

$popdbg=2;

$mnr=(int)$_GET['mnr'];

// Get mask data
sessionMask($mnr);
fxDebug($GLOBALS['fx_mask'][$mnr], 'fx_mask', $popdbg); fxDebug($GLOBALS['fx_mfields'][$mnr], 'fx_mfields', $popdbg);

// Get transaction data
$GLOBALS['_form']='';
fxSession('t','l');
fxDebug($GLOBALS['fxptdata']['chkf_'.$mnr], 'chkf', $popdbg);

// Set literals
if($GLOBALS['fxpglobals']['lang'] == 1)
{
	$headline='Checkboxen setzen für Maske: '.$GLOBALS['fx_mask'][$mnr]['text_kurz'];
	$lit_mark='Alle setzen';
	$lit_unmark='Alle entfernen';
	$lit_none='<i class=grey>(Ohne)</i>';
}
else
{
	$headline='Mark checkboxes for mask: '.$GLOBALS['fx_mask'][$mnr]['text_kurz'];
	$lit_mark='Mark all';
	$lit_unmark='Unmark all';
	$lit_none='<i class=grey>(None)</i>';
}

$ph=popupMask($headline, '', '', 'dcbpu');
echo($ph);

// Literals and fields
$afields=array();
$fields=array();
$literals=array();

// Normal mask
if($GLOBALS['fx_mask'][$mnr]['maskenart'] == 1)
{
	$c=0;
	foreach($GLOBALS['fx_mfields'][$mnr] as $fnr => $farr)
	{
		$line=$farr['zeile'];
		$column=$farr['spalte'];
		if(($line < 9000) && ($farr['anzeigeart'] == 11))	// Checkbox
		{
			foreach($GLOBALS['fxptdata']['chkf_'.$mnr] as $fieldname => $chkcnt)
			{
				$pp=strpos($fieldname,'|');
				if($pp)
				{
					$alias=substr($fieldname,$pp+1);
					$fieldname=substr($fieldname,0,$pp);
					$afields[$fieldname]=$alias;
				}
				if(strtolower($mfnfarr['feldname']) == strtolower($fieldname))
				{
					$fields[$c]=$fieldname;
					if(isset($GLOBALS['fxptdata']['litf_'.$mnr][$fieldname]))
						$literals[$c]=$GLOBALS['fxptdata']['litf_'.$mnr][$fieldname];
					else
						$literals[$c]=$farr['literal_value'];
					$c++;
				}
			}
		}
	}
fxDebug($afields,'$afields', $popdbg); fxDebug($fields,'$fields', $popdbg); fxDebug($literals,'$literals', $popdbg);

	if(sizeof($fields))
	{
		ksort($fields);
		$colcnt=sizeof($fields);
		$cwidth=floor(100/$colcnt);

		echo('<table width="100%" border="0" cellspacing="0" cellpadding="0" style="margin-top:6px;">');
		echo('	<tr>');
		// Literals
		echo('		<td width="50%" nowrap align="right" valign="middle">');
		foreach($fields as $column => $fieldname)
		{
			$l=trim($literals[$column]);
			if(!strlen($l))
				$l=$lit_none;
			echo('			&nbsp;'.$l.'&nbsp;<br />');
		}
		echo('		</td>');
		// Buttons
		echo('		<td width="50%" nowrap align="center" valign="middle">');
		echo('			<div class="fxfbs" style="text-align:left;white-space:normal;width:120px;" '.fxf_jsFunction('checkboxSet', 1, $mnr, '').'>&nbsp;<input type="checkbox" disabled checked style="position:absolute;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.$lit_mark.'&nbsp;</div>');
		echo('			<div class="fxfbs" style="text-align:left;white-space:normal;margin-top:2px;width:120px;" '.fxf_jsFunction('checkboxSet', 0, $mnr, '').'>&nbsp;<input type="checkbox" disabled style="position:absolute;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.$lit_unmark.'&nbsp;</div>');
		echo('		</td>');
		echo('	</tr>');
		echo('</table>');
	}
}
// List mask
else
{
	foreach($GLOBALS['fx_mfields'][$mnr] as $fnr => $farr)
	{
		$line=$farr['zeile'];
		$column=$farr['spalte'];
		if($line == 1)
		{
			if(isset($GLOBALS['fxptdata']['litc_'.$mnr][$column]))
				$literals[$column]=$GLOBALS['fxptdata']['litc_'.$mnr][$column];
			else
				$literals[$column]=$farr['literal_value'];
		}
		if(fxIsArray($GLOBALS['fxptdata']['chkf_'.$mnr]))
		{
			foreach($GLOBALS['fxptdata']['chkf_'.$mnr] as $fieldname => $chkcnt)
			{
				$pp=strpos($fieldname,'|');
				if($pp)
				{
					$alias=substr($fieldname,$pp+1);
					$fieldname=substr($fieldname,0,$pp);
					$afields[$fieldname]=$alias;
				}
				if(strtolower($farr['feldname']) == strtolower($fieldname))
				{
					$fields[$column]=$fieldname;
					if(isset($GLOBALS['fxptdata']['litf_'.$mnr][$fieldname]))
						$literals[$column]=$GLOBALS['fxptdata']['litf_'.$mnr][$fieldname];
				}
			}
		}
	}
fxDebug($afields,'$afields', $popdbg); fxDebug($fields,'$fields', $popdbg); fxDebug($literals,'$literals', $popdbg);

	if(sizeof($fields))
	{
		ksort($fields);
		$colcnt=sizeof($fields);
		$cwidth=floor(100/($colcnt+2));
		$bwidth=170;

		echo('<table width="100%" border="0" cellspacing="0" cellpadding="0" style="margin-top:2px;min-width:640px;">');
		// Literals
		echo('	<tr>');
		echo('		<td width="'.$cwidth.'%">&nbsp;</td>');
		$c=0;
		foreach($fields as $column => $fieldname)
		{
			$style='';
			if($c > 0)
				$style='border-left:1px solid #cccccc;';
			$l=trim($literals[$column]);
			if(!strlen($l))
			{
				$l=$lit_none;
				$style .= 'background:#f2f2f2;';
			}
			echo('	<td class="rhd" width="'.$cwidth.'%" nowrap align="center" valign="top" style="'.$style.'padding:4px 16px 2px 16px;">'.$l.'</td>');
			$c++;
		}
		echo('		<td width="'.$cwidth.'%">&nbsp;</td>');
		echo('	</tr>');
		// Buttons
		echo('	<tr>');
		echo('		<td>&nbsp;</td>');
		foreach($fields as $column => $fieldname)
		{
			if(isset($afields[$fieldname]))
				$fieldname=$afields[$fieldname];
			echo('	<td align="center" valign="top" style="padding:6px;">');
			echo('		<div class="fxfbs" style="text-align:left;white-space:normal;width:'.$bwidth.'px;height:20px;" '.fxf_jsFunction('checkboxSet', 1, $mnr, $fieldname).'>&nbsp;<input type="checkbox" disabled checked style="position:absolute;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.$lit_mark.'&nbsp;</div>');
			echo('		<div class="fxfbs" style="text-align:left;white-space:normal;margin-top:6px;width:'.$bwidth.'px;height:20px;" '.fxf_jsFunction('checkboxSet', 0, $mnr, $fieldname).'>&nbsp;<input type="checkbox" disabled style="position:absolute;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.$lit_unmark.'&nbsp;</div>');
			echo('	</td>');
		}
		echo('		<td>&nbsp;</td>');
		echo('	</tr>');
		echo('</table>');
	}
}

echo('</div>');
?>