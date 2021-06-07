<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_archivieren_db.inc                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-12-15                                                   //
// Last Change : 2020-12-15                                                   //
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
 * PF 20: Archive Projects
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if(fxIsArray($_POST) && isset($_POST['Button_Uebernehmen']))
{
	// Variablen
	$clist=array();
	$archived=0;
	$unarchived=0;
	$mc=$GLOBALS['_maskcounter'];

	if(isset($_POST['Projekt_ID']) && fxIsArray($_POST['Projekt_ID']))
	{
		reset($_POST['Projekt_ID']);
		$mc=key($_POST['Projekt_ID']);
	}

	// Liste aller übermittelten Projekte durchgehen
	foreach($_POST['Projekt_ID'][$mc] as $z => $pid)
	{
		// Alter Wert
		$old_value=$mp_list[$pid]['projekt_archiviert'];
		if(!$old_value)
			$old_value='NULL';
		else
			$old_value='1';
		// Neuer Wert
		$new_value=$_POST['projekt_archiviert'][$mc][$z];
		if(!$new_value)
			$new_value='NULL';
		else
			$new_value='1';
		// Alten und neuen Wert vergleichen und falls unterschiedlich neuen Wert merken
		if($new_value !== $old_value)
			$clist[$pid]=array('line'=>$z, 'old'=>$old_value, 'new'=>$new_value);
//echo('<b>'.$pid.':</b> $old_value='.$old_value.', $new_value='.$new_value.'<br>');
	}
fxDebug($clist, '$clist', 3);

	// Datenbankänderungen durchführen, falls Werte gefunden wurden
	if(fxIsArray($clist))
	{
		foreach($clist as $pid => $data)
		{
			$sql="UPDATE projekte SET projekt_archiviert=".$data['new']." WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
fxDebug($err, $sql, 2);
			if($err)
			{
				// Fehler beim Ändern von $var1!
				error_msg(150, false, '[<b>'.no_null($mp_list[$pid]['vorgangsnummer']).': '.$mp_list[$pid]['name_projekt'].'</b>]');
				break;
			}
			else if($data['new'] == '1')
				$archived++;
			else
				$unarchived++;
		}
	}

	// Erfolgsmeldungen
	if(!$err)
	{
		$changed=$archived+$unarchived;

		if(!$changed)
			error_msg(160);							// Es wurden keine Änderungen durchgeführt.
		else
		{
			if($changed == 1)
				error_msg(118);						// Es wurde ein Datensatz erfolgreich geändert.
			else
				error_msg(115, false, $changed);	// Es wurden $var1 Datensätze erfolgreich geändert.

			// Alle neuen Attribute in Projektliste anpassen
			foreach($clist as $pid => $data)
			{
				if($data['new'] == '1')
					$mp_list[$pid]['projekt_archiviert']='1';
				else
					$mp_list[$pid]['projekt_archiviert']='';
			}
		}
	}
}
?>