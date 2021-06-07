<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : settings_email_db.inc                                        //
// Version     : 21.1                                                         //
// Begin       : 2020-10-14                                                   //
// Last Change : 2020-10-14                                                   //
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
 * PF 246: Automated Emails - Database Actions
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

if($_POST['Button_Multi'] && is_array($pgr) && is_array($bit))
{
	reset($_POST['WerteTabID']);
	$mc=key($_POST['WerteTabID']);

	$chg=0;

	foreach($_POST['WerteTabID'][$mc] as $z => $wid)
	{
		if(!$wid)
			continue;

		$av=$GLOBALS['fxpglobals']['settings']['ems'][$wid]['ac'];
		$nv=0;
		for($c=0; $c<$cs; $c++)
		{
			if($_POST['nv_check'.$c][$mc][$z])
				$nv |= $bit[$c];
		}
fxDebug(array('$av'=>$av, '$nv'=>$nv), '$wid='.$wid, 3);
		if($av == $nv)
			continue;

		$kid=$_POST['Kategorie_ID'][$mc][$z];
		$chk="SELECT id FROM settings_email WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kategorie_id=$kid AND wertetabid=$wid";
		$id=db_value($chk);
		if(!$id)		// Neuanlage
		{
			$id=id_max('settings_email', 'id', '', '', 10000);

			$sql  = "INSERT INTO settings_email";
			$sql .= " (mandanten_id, id, kategorie_id, wertetabid, position, feld, aktionscode)";
			$sql .= " VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$id.", ".$kid.", ".$wid.", 0, 0, ".$nv.")";
		}
		else if($nv)	// Ändern
			$sql="UPDATE settings_email SET aktionscode=".$nv." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kategorie_id=".$kid." AND wertetabid=".$wid;
		else			// Löschen
			$sql="DELETE FROM settings_email WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kategorie_id=".$kid." AND wertetabid=".$wid;
		$err=db_x($sql);
fxDebug($err,$sql, 2);
		if($err)
			break;
		else
			$chg++;
	}

	// Erfolgsmeldung
	if(!$err)
	{
		if($chg == 1)
			error_msg(118, false);			// 118: Es wurde ein Datensatz erfolgreich geändert.
		else if($chg > 0)
			error_msg(115, false, $chg);	// 115: Es wurden $var1 Datensätze erfolgreich geändert.
		else
			error_msg(160, false);			// 160: Es wurden keine Änderungen durchgeführt.

		// Benutzereinstellungen aktualisieren
		$GLOBALS['fxpglobals']['settings']=benutzereinstellungen($GLOBALS['fxpglobals']['user']);
	}
}
?>