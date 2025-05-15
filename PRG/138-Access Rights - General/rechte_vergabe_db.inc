<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : rechte_vergabe_db.inc                                        //
// Version     : 24.1                                                         //
// Begin       : 2020-12-21                                                   //
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
 * PF 138: Access Rights - General
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

if($_POST['Button_Speichern'])
{
	$ama=explode(',', meldung(14,false));	//14: angezeigt,angelegt,geändert,gelöscht,hinzugefügt

	$ins=array();
	$upd=array();
	$del=array();

	if(fxIsArray($_POST['GruppeArt']))
	{
		reset($_POST['GruppeArt']);
		$akey=key($_POST['GruppeArt']);
	}
	else
		$akey=$GLOBALS['_maskcounter'];

	// Alle Einträge durchgehen
	foreach($_POST['aktionscode'][$akey] as $z => $ac)
	{
		// Alte Rechte
		if(!$ac)
			$ac=0;
		else
			$ac &= 30;

		// Neue Rechte
		$nc=0;
		if($_POST['Checkbox'][$akey][$z] || (!$ac && ($_POST['alle_auswaehlen'][$akey][$z] || $_POST['Anzeigen'][$akey][$z] || $_POST['Anlegen'][$akey][$z] || $_POST['Aendern'][$akey][$z] || $_POST['Loeschen'][$akey][$z])))
		{
			if($_POST['alle_auswaehlen'][$akey][$z])
				$nc=30;
			else
			{
				if($_POST['Anzeigen'][$akey][$z])
					$nc |= 2;
				if($_POST['Anlegen'][$akey][$z])
					$nc |= 4;
				if($_POST['Aendern'][$akey][$z])
					$nc |= 8;
				if($_POST['Loeschen'][$akey][$z])
					$nc |= 16;
			}

			if(!$nc)
				$nc=30;
		}

		// Rechte haben sich geändert?
		if($ac != $nc)
		{
//echo('['.$z.'] Rechte geändert: '.$ac.' -> '.$nc.'<br />');
			// Neuanlage
			if(!$ac)
			{
//echo('-> Neuanlage...<br />');
				$err=rechte_vergeben($rcat, $rid, $_POST['ID_Ref_Art'][$akey][$z], $nc, $_POST['GruppeArt'][$akey][$z]);
				if($err)
				{
					error_msg(3335, false, '<b>"'.$_POST['Textzeile'][$akey][$z].'"</b>', $ama[1]);	// 3335: Die Berechtigungen für $var1 konnten nicht $var2 werden!
					break;
				}
				else
					$ins[]=$_POST['Textzeile'][$akey][$z];
			}
			// Ändern
			else if($nc)
			{
//echo('-> Änderung...<br />');
				$err=rechte_vergeben(-$rcat, $rid, $_POST['ID_Ref_Art'][$akey][$z], $nc, $_POST['GruppeArt'][$akey][$z]);
				if($err)
				{
					error_msg(3335, false, '<b>"'.$_POST['Textzeile'][$akey][$z].'"</b>', $ama[2]);	// 3335: Die Berechtigungen für $var1 konnten nicht $var2 werden!
					break;
				}
				else
					$upd[]=$_POST['Textzeile'][$akey][$z];
			}
			// Löschen
			else
			{
//echo('-> Löschen...<br />');
				$err=rechte_loeschen($rcat, $rid, $_POST['ID_Ref_Art'][$akey][$z], $_POST['GruppeArt'][$akey][$z]);
				if($err)
				{
					error_msg(3335, false, '<b>"'.$_POST['Textzeile'][$akey][$z].'"</b>', $ama[3]);	// 3335: Die Berechtigungen für $var1 konnten nicht $var2 werden!
					break;
				}
				else
					$del[]=$_POST['Textzeile'][$akey][$z];
			}
		}
	}

	// Erfolgsmeldungen
	if(!$err)
	{
		$m='';
		$a=array(1=>'ins', 'upd', 'del');
		foreach($a as $ac => $at)
		{
			if(sizeof(${$at}))
			{
				if(strlen((string)$m))
					$m .= '<br />';
				$m .= $ama[$ac].'<br /><ul style="padding-left:16px;">';
				foreach(${$at} as $an)
					$m .= '<li><b>'.$an.'</b></li>';
				$m .= '</ul>';
			}
		}

		// 3337: Folgende Berechtigungen wurden erfolgreich $var1
		if(strlen((string)$m))
			error_msg(3337, false, '...<br /><br />'.$m);
		// 160: Es wurden keine Änderungen durchgeführt.
		else
			error_msg(160);
	}
//$err=true;
}
?>