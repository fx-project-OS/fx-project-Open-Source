<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_info_detail.inc                                      //
// Version     : 21.1                                                         //
// Begin       : 2021-03-30                                                   //
// Last Change : 2021-03-30                                                   //
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
 * PF 81: Time Recording (Day) - AJAX
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
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

$GLOBALS['_maskcounter']=1;
$tr=$GLOBALS['fxpglobals']['tr'];

$pid=(int)$_GET['pid'];
if(!$pid)
	$pid=0;

$mid=(int)$_GET['mid'];
if(!$mid)
	$mid=$GLOBALS['fxpglobals']['client'];

// Prüfen, ob Recht auf Person
$pids=get_persons();
$apids=explode(',', $pids);
$pid_ok=in_array($pid, $apids);

$title=meldung(1573,true);	// Zeiteintrag
$pmwidth=640;
$pmheight=128;
$pmprint=false;

if($pid_ok)
{
	// Es wurden keine passenden Einträge gefunden.
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h2 class="darkgrey">'.meldung(712,true).'</h2></td></tr></table>';

	if($_GET['zid'])
	{
		// Zeiteintrag einlesen
		$sql  = "SELECT";
		$sql .= " z.ist_beginn, z.ist_ende, z.zeitaufwand, z.pause, z.projekt_id, z.zkunde AS kunde,";
		$sql .= " z.zeitgruppe, z.freigabe, z.kw, z.ist_verzoeg, z.zeiteinheit AS zeiteinheit_ist_verz,";
		$sql .= " z.nicht_abrechenbar, z.zeitart,";
		$sql .= " t.taetigkeit_text, t.anmerkungintern AS anmerkung";
		$sql .= " FROM zeitdaten z, taetigkeiten t";
		$sql .= " WHERE z.zeit_id=".(int)$_GET['zid']." AND z.mandanten_id=".$mid." AND t.zeit_id=z.zeit_id AND t.mandanten_id=".$mid." AND z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
		$zarray=db_values($sql);
//debug2($zarray,$sql, 0);

		if(fxIsArray($zarray))
		{
			$pmwidth=-1;
			$pmheight=-1;
			$pmprint=true;

			$sum=false;
			$msk=218;
			if($zarray['zeitart'] == FXP_TE_TIMEREC_SUM)
			{
				$sum=true;
				$msk=220;
			}

			$abak=$GLOBALS['fxpglobals']['action'];
			$aktion=1;
			$GLOBALS['fxptdata']['action']=$aktion;
			$GLOBALS['fxpglobals']['action']=$aktion;

			$maskenfeldwerte=$zarray;

			// Datum
			$maskenfeldwerte['datum']=$maskenfeldwerte['ist_beginn'];
			// Zus. Aufwand
			if($maskenfeldwerte['ist_verzoeg'] == 0.00)
				$maskenfeldwerte['ist_verzoeg']='';
			// nicht verrechenbar
			if(!$maskenfeldwerte['nicht_abrechenbar'])
				$maskenfeldwerte['nicht_abrechenbar']='';
			// Startzeit + Endzeit
			if($maskenfeldwerte['ist_beginn'] != $maskenfeldwerte['ist_ende'])
			{
				$maskenfeldwerte['ist_beg_zeit']=$maskenfeldwerte['ist_beginn'];
				$maskenfeldwerte['ist_end_zeit']=$maskenfeldwerte['ist_ende'];
				$maskenfeldwerte['zeitaufwand']='';
			}

			if($sum)
				$maskenfeldwerte['startdatum']=$maskenfeldwerte['ist_beginn'];

			$inhalt=maske($msk);

			$aktion=$abak;
			$GLOBALS['fxptdata']['action']=$aktion;
			$GLOBALS['fxpglobals']['action']=$aktion;
		}
	}
}
else
{
	// Sie haben nicht die Berechtigung, diese Programmfunktion aufzurufen.
	$inhalt='<table width=100% height=100%><tr><td width=100% align=center><h1 class="red">'.meldung(413,true).'</h1></td></tr></table>';
}

echo(popupMask($title, '', $inhalt, 'dtxsi', $pmwidth, $pmheight, $pmprint));
?>