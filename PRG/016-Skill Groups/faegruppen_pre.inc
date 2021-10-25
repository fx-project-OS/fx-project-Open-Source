<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : faegruppen_pre.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-10-19                                                   //
// Last Change : 2020-10-19                                                   //
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
 * PF 16: Skill Groups
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$err)
	unset($GLOBALS['fxptdata']['lvalues']);

// Index setzen
$index89=true;
$mc=$GLOBALS['_maskcounter'];
$lcnt=0;

// Height
$MaArt_Name_height=22;

// No right border in the following column
fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

// Felder füllen
$sql="SELECT DISTINCT maart_name, maart_id, mandanten_id FROM ma_art WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY maart_name";
$GLOBALS['fxptdata']['lvalues'][$mc]=db_values($sql, '*');
if(!fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
	$GLOBALS['fxptdata']['lvalues'][$mc]=array();

// Anzeige vorbereiten
switch($GLOBALS['fxpglobals']['action'])
{
	case FXP_DISPLAY:
		if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
		{
			for($lcnt=0; $lcnt<sizeof($GLOBALS['fxptdata']['lvalues'][$mc]); $lcnt++)
			{
				if($GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['mandanten_id'] == $GLOBALS['fxpglobals']['client'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='~'.$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'];
			}
		}

		$Checkbox_nv=true;
		$Button_Multi_nv=true;
	break;

	case FXP_CREATE:
		// Liste anzeigen
		if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
		{
			for($lcnt=0; $lcnt<sizeof($GLOBALS['fxptdata']['lvalues'][$mc]); $lcnt++)
			{
				if($GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['mandanten_id'] == $GLOBALS['fxpglobals']['client'])
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='~'.$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'];
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='~<font class="grey">'.$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'].'</font>';
			}
		}

		// sowie Leereinträge entspr. Einstellungswerten
		if($lcnt)
		{
			fxf_maskColumnBorder(array('Checkbox|0','MaArt_Name'), 1, -1, $mc, $lcnt, true);
			$lcnt++;

			$headline='+';
			$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['checkbox']='~|';
			$GLOBALS['fxptdata']['lvalues'][$mc][$li]['maart_name']=$headline;
			fxf_listSectHeader($mc, $lcnt, '&nbsp;<font class="s4b">+</font>', 'maart_name', 'maart_name', 1, -2, 1);
			$lcnt++;
		}

		$ln=max(1, $GLOBALS['fxpglobals']['settings']['listenneu']);
		for($lc=0; $lc<$ln; $lc++)
		{
			$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_id']=-1;
			if($err && strlen($MaArt_Name[$mc][$lcnt]))
				$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']=$MaArt_Name[$mc][$lcnt];
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='';
			$lcnt++;
		}

		$Checkbox_nv=true;
	break;

	case FXP_CHANGE:
		$anzahl=0;
		if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
		{
			for($lcnt=0; $lcnt<sizeof($GLOBALS['fxptdata']['lvalues'][$mc]); $lcnt++)
			{
				if($err)
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']=$MaArt_Name[$mc][$lcnt];
				if(($GLOBALS['fxpglobals']['client'] > 1) && ($GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['mandanten_id'] != $GLOBALS['fxpglobals']['client']))
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='~<font class="grey">'.$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'].'</font>';
				else
					$anzahl++;
				if($err)
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['dummy']=$Dummy[$mc][$lcnt];
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['dummy']=$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'];
			}
		}

		$Checkbox_nv=true;
		if(!$anzahl)
			$Button_Multi_nv=true;
	break;

	case FXP_DELETE:
		$maske89_sf=true;
		$anzahl=0;
		if(fxIsArray($GLOBALS['fxptdata']['lvalues'][$mc]))
		{
			for($lcnt=0; $lcnt<sizeof($GLOBALS['fxptdata']['lvalues'][$mc]); $lcnt++)
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['dummy']=$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'];
				if(($GLOBALS['fxpglobals']['client'] > 1) && ($GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['mandanten_id'] != $GLOBALS['fxpglobals']['client']))
				{
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['checkbox']='~&nbsp;';
					$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='~<font class="grey">'.$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'].'</font>';
				}
				else
				{
					$anzahl++;
					if($err && $Checkbox[$mc][$lcnt])
						$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['checkbox']=1;
					if($GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['mandanten_id'] == $GLOBALS['fxpglobals']['client'])
						$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name']='~'.$GLOBALS['fxptdata']['lvalues'][$mc][$lcnt]['maart_name'];
				}
			}
		}
		if(!$anzahl)
		{
			$Checkbox_nv=true;
			$Button_Multi_nv=true;
		}
		else
			$Checkbox_sf=0;
	break;
}

if($lcnt)
{
	fxf_maskColumnBorder(array('Checkbox|0','MaArt_Name'), 1, -1, $mc, $lcnt, true);
	$lcnt++;
}

// Adjust width if checkbox is not visible
if(isset($Checkbox_nv) && $Checkbox_nv)
{
	$fw=(int)abs(db_value("SELECT anzeigegroesse1 FROM maskenfelder WHERE nummer=89 AND eingabefeld=374 AND zeile>1"));
	if($fw)
		$MaArt_Name_width=(int)($fw+26);
}
?>