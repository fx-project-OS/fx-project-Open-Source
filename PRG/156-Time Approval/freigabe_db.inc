<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : freigabe_db.inc                                              //
// Version     : 21.1                                                         //
// Begin       : 2020-11-12                                                   //
// Last Change : 2020-11-12                                                   //
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
 * PF 156: Time Approval
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$dbg_reldb=false;
if($db_update && $_POST['Button_Speichern'])
{
//fxDebug('_POST');
	$Zeitspanne_von=substr($Zeitspanne_von,0,8).'000000';
	$Zeitspanne_bis=substr($Zeitspanne_bis,0,8).'240000';

	$mr=$GLOBALS['fxpglobals']['persdat']['role']+1;		// MA->1, PL->2, GF->3
	if($GLOBALS['fxpglobals']['persdat']['roles'][-1])
		$mr=3;												// VW->3

	$fg=array();
	$ub=array();
	$upd=0;
	$upd_f=0;
	$upd_t=0;
	$upd_r=0;
	$upd_m=0;
	$del=0;

	// Änderungen ermitteln
	if(fxIsArray($_POST['nv_ganzzahl1']))
	{
		reset($_POST['nv_ganzzahl1']);
		$mc=key($_POST['nv_ganzzahl1']);

		foreach($_POST['nv_ganzzahl1'][$mc] as $l => $z)
		{
			// Zeit-ID (für Zeitdaten und Reisekosten)
			$zid=(int)$z;
			// ID (für Material)
			if(fxIsArray($_POST['nv_ganzzahl2']) && fxIsArray($_POST['nv_ganzzahl2'][$mc]) && isset($_POST['nv_ganzzahl2'][$mc][$l]) && strlen($_POST['nv_ganzzahl2'][$mc][$l]))
				$mid=(int)$_POST['nv_ganzzahl2'][$mc][$l];
			else
				$mid=0;
			// Typ
			if(($zid < 0) && ($mid > 0))
				$typ='MS';
			else if(($zid == 0) && ($mid > 0))
				$typ='MV';
			else if(($zid > 0) && ($mid == 0))
				$typ='ZT';
			else
				$typ='';
			// Projekt-ID
			if(fxIsArray($_POST['nv_ganzzahl2']) && fxIsArray($_POST['nv_ganzzahl2'][$mc]) && isset($_POST['nv_ganzzahl2'][$mc][$l]) && strlen($_POST['nv_ganzzahl2'][$mc][$l]))
				$pid=(int)$_POST['nv_ganzzahl2'][$mc][$l];
			else
				$pid=0;
			// Nicht abrechenbar (Zeitdaten+Reisekosten+Material)
			$nbs='000';
			if(fxIsArray($_POST['nv_text1']) && fxIsArray($_POST['nv_text1'][$mc]) && isset($_POST['nv_text1'][$mc][$l]) && strlen($_POST['nv_text1'][$mc][$l]))
				$nbs=$_POST['nv_text1'][$mc][$l];

			if(strlen($typ))
			{
				$id=$typ;
				if($zid > 0)
					$id .= $zid;
				else
					$id .= $mid;
if($dbg_reldb) echo($l.': $zid='.$zid.', $mid='.$mid.' -- $id='.$id.'<br />');
				// ...Zeitdatenfreigabe
				if(fxIsArray($_POST['Checkbox']) && fxIsArray($_POST['Checkbox'][$mc]) && isset($_POST['Checkbox'][$mc][$l]) && $_POST['Checkbox'][$mc][$l])
				{
if($dbg_reldb) echo('...<b>Zeitdaten-, Reisekosten oder Materialfreigabe</b><br />');
					if(!isset($fg[$id]))
						$fg[$id]=array('typ'=>$typ, 'zid'=>$zid, 'mid'=>$mid);
					$fg[$id]['set']=true;
				}
				// ...Zeitdatenfreigabe rückgängig machen
				if(fxIsArray($_POST['oldfreigabe']) && fxIsArray($_POST['oldfreigabe'][$mc]) && isset($_POST['oldfreigabe'][$mc][$l]))
				{
					$of=(int)$_POST['oldfreigabe'][$mc][$l];
					if(!fxIsArray($_POST['delchk']) || !fxIsArray($_POST['delchk'][$mc]) || !isset($_POST['delchk'][$mc][$l]) || !$_POST['delchk'][$mc][$l])
					{
if($dbg_reldb) echo('...<b>Zeitdaten-, Reisekosten- oder Materialfreigabe rückgängig machen</b>: '.$of.'<br />');
						if(!isset($fg[$id]))
							$fg[$id]=array('typ'=>$typ, 'zid'=>$zid, 'mid'=>$mid);
						$fg[$id]['undo']=$of;
					}
				}
				// ...Zeitdaten, Reisekosten oder Material nicht abrechenbar
				$tnbr=(int)substr($nbs,0,1);
				if($tnbr)
					$tnbr=1;
				else
					$tnbr=0;
				$rnbr=(int)substr($nbs,1,1);
				if($rnbr)
					$rnbr=1;
				else
					$rnbr=0;
				$mnbr=(int)substr($nbs,2,1);
				if($mnbr)
					$mnbr=1;
				else
					$mnbr=0;
				$tnb=0;
				if(fxIsArray($_POST['nicht_abrechenbar']) && fxIsArray($_POST['nicht_abrechenbar'][$mc]) && isset($_POST['nicht_abrechenbar'][$mc][$l]) && $_POST['nicht_abrechenbar'][$mc][$l])
					$tnb=1;
				$rnb=0;
				if(fxIsArray($_POST['rk_nicht_abrechenbar']) && fxIsArray($_POST['rk_nicht_abrechenbar'][$mc]) && isset($_POST['rk_nicht_abrechenbar'][$mc][$l]) && $_POST['rk_nicht_abrechenbar'][$mc][$l])
					$rnb=1;
				$mnb=0;
				if(fxIsArray($_POST['mat_nicht_abrechenbar']) && fxIsArray($_POST['mat_nicht_abrechenbar'][$mc]) && isset($_POST['mat_nicht_abrechenbar'][$mc][$l]) && $_POST['mat_nicht_abrechenbar'][$mc][$l])
					$mnb=1;
if($dbg_reldb) echo($l.': $nbs='.$nbs.' -- $tnbr='.$tnbr.', $tnb='.$tnb.' -- $rnbr='.$rnbr.', $rnb='.$rnb.' -- $mnbr='.$mnbr.', $mnb='.$mnb.'<br />');
				if(($tnbr != $tnb) || ($rnbr != $rnb) || ($mnbr != $mnb))
				{
					if(!isset($fg[$id]))
						$fg[$id]=array('typ'=>$typ, 'zid'=>$zid, 'mid'=>$mid);
					if($tnbr != $tnb)
					{
if($dbg_reldb) echo('...<b>Zeitdaten nicht abrechenbar</b>: $tnbr='.$tnbr.' != $tnb='.$tnb.'<br />');
						$fg[$id]['tnb']=$tnb;
					}
					if($rnbr != $rnb)
					{
if($dbg_reldb) echo('...<b>Reisekosten nicht abrechenbar</b>: $rnbr='.$rnbr.' != $rnb='.$rnb.'<br />');
						$fg[$id]['rnb']=$rnb;
					}
					if($mnbr != $mnb)
					{
if($dbg_reldb) echo('...<b>Material nicht abrechenbar</b>: $mnbr='.$mnbr.' != $mnb='.$mnb.'<br />');
						$fg[$id]['mnb']=$mnb;
					}
					if($pid > 0)
						$ub[$pid]=true;
				}
			}
		}
	}
if($dbg_reldb) { fxDebug($fg, '$fg'); fxDebug($ub, '$ub'); }

	// Änderungen durchführen
	if(!$err && sizeof($fg))
	{
		foreach($fg as $id => $za)
		{
if($dbg_reldb) fxDebug($za, $id);
			$typ=$za['typ'];
			$zid=(int)$za['zid'];
			$mid=(int)$za['mid'];

			$sql="SELECT freigabe FROM zeitdaten_freigabe WHERE zeit_id=".$zid." AND id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY freigabe DESC";
			$mf=(int)db_value($sql);
if($dbg_reldb) fxDebug($mf, $sql);
			// ...Freigabe rückgängig machen
			if(isset($za['undo']))
			{
				$of=(int)$za['undo'];
				if($mf <= $of)
				{
					$sql="DELETE FROM zeitdaten_freigabe WHERE zeit_id=".$zid." AND id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND freigabe=".$of;
					$err=db_x($sql);
if($dbg_reldb) fxDebug($err, $sql);
					if(!$err)
					{
						$sql="SELECT freigabe FROM zeitdaten_freigabe WHERE zeit_id=".$zid." AND id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY freigabe DESC";
						$mf=(int)db_value($sql);
if($dbg_reldb) fxDebug($mf, $sql);
						if(!$mf)
							$mf="NULL";

						if($typ == 'MS')
							$sql="UPDATE sachmittel SET freigabe=".$mf." WHERE sachmittel_id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						else if($typ == 'MV')
							$sql="UPDATE materialverbrauch SET freigabe=".$mf." WHERE id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						else
							$sql="UPDATE zeitdaten SET freigabe=".$mf." WHERE zeit_id=".$zid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
if($dbg_reldb) fxDebug($err, $sql);
					}
					if($err)
					{
						error_msg(1674, false, $id);	// 1674: Fehler beim Ändern des Freigabestatus für Datensatzes $var1!
						break;
					}
					else
						$del++;
				}
			}
			// ...Freigabe Änderung
			else
			{
				$zc="";
				if(isset($za['set']) && ($mf <= $mr))
					$zc .= ", freigabe=".$mr;
				if($typ == 'ZT')
				{
					if(isset($za['tnb']))
					{
						$zc .= ", nicht_abrechenbar=";
						if($za['tnb'])
							$zc .= "1";
						else
							$zc .= "NULL";
					}
					if(isset($za['rnb']))
					{
						$zc .= ", rk_nicht_abrechenbar=";
						if($za['rnb'])
							$zc .= "1";
						else
							$zc .= "NULL";
					}
				}
				else if(($typ == 'MV') && isset($za['mnb']))
				{
					$zc .= ", nicht_abrechenbar=";
					if($za['mnb'])
						$zc .= "1";
					else
						$zc .= "NULL";
				}
				if(strlen($zc))
				{
					if($typ == 'MS')
						$sql="UPDATE sachmittel SET".substr($zc,1)." WHERE sachmittel_id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					else if($typ == 'MV')
						$sql="UPDATE materialverbrauch SET".substr($zc,1)." WHERE id=".$mid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					else
						$sql="UPDATE zeitdaten SET".substr($zc,1)." WHERE zeit_id=".$zid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
if($dbg_reldb) fxDebug($err, $sql);
					if(!$err && isset($za['set']) && ($mf <= $mr))
					{
						$sql="INSERT INTO zeitdaten_freigabe (mandanten_id, zeit_id, id, freigabe) VALUES (".$GLOBALS['fxpglobals']['client'].", ".$zid.", ".$mid.", ".$mr.")";
						$err=db_x($sql);
if($dbg_reldb) fxDebug($err, $sql);
					}
					if($err)
					{
						error_msg(1674, false, $id);	// 1674: Fehler beim Ändern des Freigabestatus oder der Abrechenbarkeit für Datensatzes '$var1'!
						break;
					}
					else
					{
						$upd++;
						if(isset($za['set']) && ($mf <= $mr))
							$upd_f++;
						if(isset($za['tnb']))
							$upd_t++;
						if(isset($za['rnb']))
							$upd_r++;
						if(isset($za['mnb']))
							$upd_m++;
					}
				}
			}
		}
	}

	// Budgetkalkulationen durchführen
	if(!$err && sizeof($ub))
	{
		foreach($ub as $pid => $dummy)
		{
			$err=update_budget($pid, array());
if($dbg_reldb) fxDebug($err, 'update_budget('.$pid.')');
			if($err)
			{
				error_msg($err);
				break;
			}
		}
	}

	if(!$err)
	{
		if($upd_t || $upd_r || $upd_m)
			error_msg(2839);	// 2839:	Der Abrechenbarkeitsstatus aller markierten Datensätze wurde erfolgreich geändert.
		if($del)
			error_msg(290);		// 290:		Alle markierten freigegebenen Datensätze wurden erfolgreich zurückgesetzt.
		if($upd_f)
			error_msg(1675);	// 1675:	Alle markierten Datensätze wurden erfolgreich freigegeben.
		if(!$upd && !$del)
			error_msg(160);		// 160:		Es wurden keine Änderungen durchgeführt.
	}
if($dbg_reldb) $err=true;
}
?>