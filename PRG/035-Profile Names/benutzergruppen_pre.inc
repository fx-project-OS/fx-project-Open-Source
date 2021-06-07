<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benutzergruppen_pre.inc                                      //
// Version     : 21.1                                                         //
// Begin       : 2020-10-12                                                   //
// Last Change : 2020-10-12                                                   //
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
 * PF 35: Profile Names - List Values
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Mask prepearation variables
$index=true;
$mc=$GLOBALS['_maskcounter'];
$l=0;
$GLOBALS['fxptdata']['lvalues'][$mc]=array();

// No checkbox popup in Display and Create mode
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	$GLOBALS['maske'.$GLOBALS['_masknumber'].'_ncb']=true;

// Height
$Gruppenname_height=26;

// Get client's profile groups
$ca=fxf_getClientData();
$pga=$ca['pga'];
fxDebug($pga,'$pga: Profile groups', 3);

// Change Mode?
if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
	$maske73_sf=true;

// Buttons
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv=true;

// No right border in the following column
fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

// Cycle through all existing groups
if(fxIsArray($pga))
{
	foreach($pga as $gid => $ga)
	{
		// Checkbox
		if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
		{
			if($ga['act'])
				$img='1';
			else
				$img='x';
			$lw='~<img src="'.$GLOBALS['gfxpath'].$img.'.png">';
		}
		else if($err)
			$lw=$_POST['Checkbox'][$mc][$l];
		else if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $ga['act'])
			$lw='1';
		else
			$lw='';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']=$lw;

		// Group Name
		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			if($ga['cnt'])
				$lw='~'.$ga['nam'];
			else
				$lw='~<font class="grey">'.$ga['nam'].'</font>';
		}
		else if($err)
			$lw=trim($_POST['Gruppenname'][$mc][$l]);
		else
			$lw=$ga['nam'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['gruppenname']=$lw;

		// No.
		$lw=get_entry($ga['cnt'], 'ganzzahl_tz').'x';
		if(!$ga['cnt'])
			$lw='<font class="grey">'.$lw.'</font>';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['anz']='~'.$lw.'&nbsp;';

		// (Hidden)
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['benutzergr_id']=max(0, (int)$gid);
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['gruppenname_id']=(int)$ga['nid'];
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_wert']=trim($ga['nam']);

		$l++;
	}

	// Empty line with top border
	fxf_maskColumnBorder(array('Checkbox|0', 'Gruppenname', 'anz'), 1, -1, $mc, $l, true);
	$l++;
}

// New group(s)
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	// x new lines
	$neutr=$GLOBALS['fxpglobals']['settings']['listenneu'];
	if($neutr < 1)
		$neutr=1;
	for($lzt=0; $lzt<$neutr; $lzt++)
	{
		// Checkbox
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['checkbox']='~<span class="s4 bold darkgrey">+</span>';

		// Group Name
		if($err)
			$lw=$_POST['Gruppenname'][$mc][$l];
		else
			$lw='';
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['gruppenname']=$lw;

		// No.
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['anz']='~<i class="lightergrey">0x</i>&nbsp;';

		// (Hidden)
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['benutzergr_id']=0;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['gruppenname_id']=0;
		$GLOBALS['fxptdata']['lvalues'][$mc][$l]['name_wert']='';

		$l++;
	}

	// Empty line with top border
	fxf_maskColumnBorder(array('Checkbox|0', 'Gruppenname', 'anz'), 1, -1, $mc, $l, true);
	$l++;
}
?>