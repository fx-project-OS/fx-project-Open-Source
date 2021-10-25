<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : nachschlage_db.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2020-07-22                                                   //
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
 * PF 58: Reference Values - Database file for saving new or changed reference values
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$ID_Feld=(int)$ID_Feld;	// komische Sache, aber der Integer wurde irgendwo zu einem String abgeändert???

// Änderungsarray wird in Validierung erstellt
if(sizeof($ca))
{
	// Variablen leeren
	$ins=array();	// ...Inserts
	$upd=array();	// ...Updates
	$del=array();	// ...Deletes

	foreach($ca as $c)
	{
//fxDebug($c,'$c');
		$es=clientLookupValue($c['wertetabid'], $c['tabwert'], $c['position'], $ID_Feld);
		if(substr($es,0,1) == 'E')	// Fehler
		{
			$msg=substr($es,1);
			$GLOBALS['fxpvars']['msgarr']=array(300000=>array(0=>array('nr'=>$tr.'-'.$aktion, 'val'=>$msg)));
			$err=true;
			break;
		}
		else						// Erfolg
		{
			if(!$c['wertetabid'])
				$ins[]=$c;
			else if(!strlen($c['tabwert']))
				$del[]=$c;
			else
				$upd[]=$c;
		}
	}

	if(!$err)	// Erfolgsmeldung
	{
		$m='';
		$ama=explode(',', meldung(14,false));	//14: angezeigt,angelegt,geändert,gelöscht,hinzugefügt
		$a=array(1=>'ins', 'upd', 'del');
		foreach($a as $ac => $at)
		{
			if(sizeof(${$at}))
			{
				if(strlen($m))
					$m .= '<br />';
				$m .= $ama[$ac].'<br /><ul style="padding-left:16px;">';
				foreach(${$at} as $an)
					$m .= '<li><b>'.$an['wert'].'</b></li>';
				$m .= '</ul>';
			}
		}

		// 3340: Folgende $var1 wurden erfolgreich $var2
		if(strlen($m))
		{
			error_msg(3340, false, meldung(853,false), '...<br /><br />'.$m);	// Nachschlagewerte
		}
		// 160: Es wurden keine Änderungen durchgeführt.
		else
			error_msg(160);
	}
}
else if($_POST['Button_Speichern'])
{
	if($aktion == FXP_CREATE)
		error_msg(168);		// 168: Es wurden keine Daten für die Neuanlage eingegeben.
	else if($aktion == FXP_DELETE)
		error_msg(1642);	// 1642: Es wurden keine Einträge angehakt.
	else
		error_msg(160);		// 160: Es wurden keine Änderungen durchgeführt.
}
//$err=true;
?>