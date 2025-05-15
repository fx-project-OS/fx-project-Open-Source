<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : benzugr_pre.inc                                              //
// Version     : 24.1                                                         //
// Begin       : 2020-10-13                                                   //
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
 * PF 97: User Profile Assignment - Assignment Mask
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$index=true;
$mc=$GLOBALS['_maskcounter'];

switch($Modi)
{
	case 1: // Benutzergruppenmodus
		$Benutzergr_ID_arg=array('Switch'=>1);
		$ges='benutzer_id';
		$ist='benutzergr_id';
	break;

	case 2: // Benutzermodus
		$Benutzergr_ID_arg=array('Switch'=>0);
		$ges='benutzergr_id';
		$ist='benutzer_id';
	break;
}
// Benutzer/Benutzergruppen via Funktion holen
$ben=auswahl_Benutzergr_ID(1);

// Zuordnung holen
$zuordnung=array();
$sql="SELECT b.id_bgz, b.".$ges." FROM benutzergruppenzo b WHERE b.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.".$ist."=".$Benutzergr_ID;
$bgz=db_values($sql, '*');
if(fxIsArray($bgz))
{
	foreach($bgz as $bval)
		$zuordnung[$bval[$ges]]=$bval['id_bgz'];
}

// Arrays verknüpfen
if(fxIsArray($ben))
{
fxDebug($ben,'$ben', 2);

	// No right border in the following column
	fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

	$i=0;
	foreach($ben as $bid => $bname)
	{
		if($bname == '<d>')
			continue;

		if($Modi == 1)
		{
			$ba=selGetStyle($bname);
//fxDebug($ba,$bname, 0);
			// Personentyp?
			if($ba['dis'])
			{
				if($i)
				{
					fxf_maskColumnBorder(array('Checkbox','Name_Benutzer'), 1, -1, $mc, $i, true);
					$i++;
				}
				fxf_listSectHeader($mc, $i, '&nbsp;<font class="s4b">'.$ba['tx'].'</font>', 'checkbox', 'name_benutzer', 2, -2, 1);
				$i++;

				continue;
			}
			$sbp=strpos($ba['tx'],'[');
			if($sbp)
				$ba['tx']='<b>'.substr((string)$ba['tx'],0,$sbp).'</b>'.substr((string)$ba['tx'],$sbp);
			$bname='<span tooltip="'.$ba['tt'].'">'.$ba['tx'].'</span>';
		}

		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='^class_?|';
		if($err)
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox'] .= $_POST['Checkbox'][$mc][$i];
		else if($zuordnung[$bid])
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox'] .= '1';

		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['name_benutzer']='~'.$bname;

		// Hidden
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['benutzer_id']=$bid;
		if($zuordnung[$bid])
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['id_bgz']=$zuordnung[$bid];
		else
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['id_bgz']=0;
		$i++;
	}

	if($i)
	{
		fxf_maskColumnBorder(array('Checkbox','Name_Benutzer'), 1, -1, $mc, $i, true);
		$i++;
	}
}
?>