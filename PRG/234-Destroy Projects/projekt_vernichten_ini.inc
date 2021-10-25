<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projekt_vernichten_ini.inc                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-12-16                                                   //
// Last Change : 2020-12-16                                                   //
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
 * PF 234: Destroy Projects
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

setForm(false);

$maske475_sf=true;								// Maske auch im Lösch-Modus bearbeitbar
$mid=$GLOBALS['fxpglobals']['client'];	// Mandanten-Nummer
$pinf=array();

// Debug eingeschaltet?
if($GLOBALS['__DGBLVL'] && ($GLOBALS['__DGBLVL'] < 5))
	$pv_debug=true;
else
	$pv_debug=false;
//$pv_debug=true;

// Auf irgendeinen Button geklickt?
if(($_POST['Button_Select'] || $_POST['button_destroy']) && $_POST['Projekt_ID'])
{
	$Projekt_ID=(int)$_POST['Projekt_ID'];

	// Vorgangsnummer ermitteln
	$vn=trim(db_value("SELECT vorgangsnummer FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$mid));
	if(strlen($vn))
	{
		$sqlz=" FROM projekte WHERE mandanten_id=".$mid." AND vorgangsnummer LIKE '".$vn."%'";

		$pinf['vn']=$vn;
		$pinf['pid']="SELECT projekt_id".$sqlz;

		$pinf['sub']=(int)db_value("SELECT COUNT(projekt_id) AS cnt".$sqlz." AND projektart=".FXP_PRJ_SUB);
		$pinf['tsk']=(int)db_value("SELECT COUNT(projekt_id) AS cnt".$sqlz." AND projektart=".FXP_PRJ_TASK);

		$pinf['tab']=array();
	}
}

if(sizeof($pinf))
{
	$zmust=array(
		'auftrag'						=> 2,
		'd_dokumente'					=> 1,
		'ekvk'							=> 1,
		'maeinsatz'						=> 1,
		'maeinsatz_history'				=> 1,
		'maplanung'						=> 1,
		'maplanung_history'				=> 1,
		'materialverbrauch'				=> 1,
		'materialverbrauch_history'		=> 1,
		'rechnungsposition'				=> 1,
		'sachmittel'					=> 1,
		'sachmittel_history'			=> 1,
		'termine'						=> 1,
		'termine_history'				=> 1,
		'zeitdaten'						=> 1,
		'zeitdaten_history'				=> 1
	);
	$zinf=delProjectArray($pinf['pid'],true,true);
//fxDebug($zinf,'$zinf');
	foreach($zinf as $tab => $tsql)
	{
		// COUNT
		if(substr($tsql,0,6) == 'DELETE')
			$csql="SELECT COUNT(*)".substr($tsql,6);
		else
		{
			$wp=strpos($tsql,' WHERE');
			$csql="SELECT COUNT(*) FROM ".$tab.substr($tsql,$wp);
		}
		$cval=(int)db_value($csql);
//fxDebug($cval,$csql);

		// MUST
		$must=0;
		if(isset($zmust[$tab]))
			$must=$zmust[$tab];
		$pinf['tab'][$tab]=array('cnt'=>$cval, 'del'=>$tsql, 'must'=>$must);
	}
}
//fxDebug($pinf, '$pinf');

// Projektdaten löschen
$da=array();
if(sizeof($pinf) && $_POST['button_destroy'] && !$err)
{
	if(fxIsArray($pinf['tab']))
	{
		// Get all project relevant documents
		$da=delProjectDocsArray($pinf['pid'],$da,false);
//fxDebug($da,'$da');

		foreach($pinf['tab'] as $tabname => $tarr)
		{
			if($tarr['cnt'] > 0)
			{
				if($pv_debug)
					fxDebug($tarr['del'], $tabname);
				else
					$err=db_x($tarr['del']);

				if($err)
					break;
			}
		}
	}

	if($err)
	{
		error_msg(431, 0, $tabname);	// 431: Fehler beim Löschen der abhängigen Projektdaten in der Tabelle [$var1]! Die Aktion wurde abgebrochen.
		if(!$pv_debug)
			db_rollback('rollback');
	}
	else
	{
		// Delete documents
		if(sizeof($da))
			delProjectDocsArray(false,$da);

		error_msg(432, false);			// 432: Das komplette Mutterprojekt inkl. aller Abhängigkeiten wurde erfolgreich aus der Datenbank entfernt.
		if(!$pv_debug)
		{
			// Protokoll schreiben
			$forceprotocol=true;
			db_protokoll('write', 'Program "'.basename(__FILE__).'": Project '.no_null($pinf['vn']).' ['.$Projekt_ID.'] incl. all dependencies has been destroyed by: Person ID='.$GLOBALS['fxpglobals']['person'].', User ID='.$GLOBALS['fxpglobals']['user']);

			db_rollback('ok');
			$pinf=array();
			$maske475_nv=true;
		}
	}
}

if(!sizeof($pinf))
	$maske475_nv=true;
?>