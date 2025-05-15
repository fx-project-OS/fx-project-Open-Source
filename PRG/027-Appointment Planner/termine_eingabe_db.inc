<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_eingabe_db.inc                                       //
// Version     : 24.1                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2024-07-04                                                   //
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
 * PF 27: Appointment Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$tidarr=array();
$dolist=false;
if(($lkey > 0) && ($Termin_ID == -1))
{
	$dolist=true;
	if(strlen((string)$simg) && ($siid > 0))
		$tidarr[$siid]=$spersid;
	else
	{
		foreach($_POST['nv_text1'][$lkey] as $l => $si)
		{
			if($_POST['Checkbox'][$lkey][$l])
			{
				$sie=explode('_', $si);
				$tidarr[(int)$sie[1]]=(int)$sie[0];
			}
		}
	}
}
else if(($Termin_ID > 0) || ($GLOBALS['fxpglobals']['action'] == FXP_CREATE))
	$tidarr[(int)$Termin_ID]=$persid;
//fxDebug($tidarr, '$tidarr');

if(sizeof($tidarr))
{
	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];

	// Variablen leeren
	$GLOBALS['_ua']=array();
	$GLOBALS['_ma']=array();

	$inscnt=0;
	$updcnt=0;
	$delcnt=0;

	// Get sender data
	$sender=$GLOBALS['fxpglobals']['person'];
	if(!isset($GLOBALS['_ua'][$sender]))
	{
		$pedata=get_email_adress($sender,true,true);
		if(fxIsArray($pedata) && fxIsArray($pedata[$sender]))
			$GLOBALS['_ua'][$sender]=$pedata[$sender];
		else
			$GLOBALS['_ua'][$sender]=false;
	}
	if(fxIsArray($GLOBALS['_ua'][$sender]))
		$sender_name=$GLOBALS['_ua'][$sender]['name2'];
	else
		$sender_name='('.$sender.')';
//fxDebug($GLOBALS['_ua'][$sender], '$sender='.$sender.' -- '.$sender_name);
//echo('$sender='.$sender.' -- '.$sender_name.'<hr />'.$nl);

	foreach($tidarr as $Termin_ID => $applicant)
	{
		// Get applicant data (= recipient)
		if(!isset($GLOBALS['_ua'][$applicant]))
		{
			$pedata=get_email_adress($applicant,true,true);
			if(fxIsArray($pedata) && fxIsArray($pedata[$applicant]))
				$GLOBALS['_ua'][$applicant]=$pedata[$applicant];
			else
				$GLOBALS['_ua'][$applicant]=false;
		}
		if(fxIsArray($GLOBALS['_ua'][$applicant]))
			$applicant_name=$GLOBALS['_ua'][$applicant]['name2'];
		else
			$applicant_name='('.$applicant.')';
//fxDebug($GLOBALS['_ua'][$applicant], '$applicant='.$applicant.' -- '.$applicant_name);
//echo('$applicant='.$applicant.' -- '.$applicant_name.'<hr />'.$nl);

		// ...Mastertermin-ID
		$MasterTermin_ID=0;
		if($Termin_ID > 0)
			$MasterTermin_ID=db_value("SELECT mastertermin_id FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		// Sonderdaten ermitteln bei Liste
		if($dolist)
		{
			// ...Bestandsdaten holen
			if($Termin_ID > 0)
			{
				$sql="SELECT * FROM termine WHERE termin_id=".(int)$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$old=db_values($sql);
			}
			else
				$old=false;

			// ...Zeiterfassungstermin
			if(fxIsArray($old))
				$zeit_id=(int)$old['zeit_id'];
			else
				$zeit_id=0;

			// ...Vorgang
			if(fxIsArray($old))
				$sel_projekt_id=(int)$old['projekt_id'];
			else
				$sel_projekt_id=0;

			// ..._POST
			$_POST['Termin_ID']=$Termin_ID;
			$_POST['MasterTermin_ID']=$MasterTermin_ID;
			$_POST['Treffpunkt']="";
			$_POST['Name_Termin']="";
			$_POST['Beschreibung']="";
			if(fxIsArray($old))
			{
				$_POST['Treffpunkt']=$old['treffpunkt'];
				$_POST['Name_Termin']=$old['name_termin'];
				$_POST['Beschreibung']=$old['beschreibung'];
			}

			// ...mfw_backup
			$mfw_backup=array(
				'treffpunkt'=> $_POST['Treffpunkt'],
				'name_termin'=> $_POST['Name_Termin'],
				'beschreibung'=> $_POST['Beschreibung'],
			);
			if(fxIsArray($old))
			{
				$mfw_backup['terminart']=$old['terminart'];
				$mfw_backup['soll_beg_dtm']=$old['startzeit'];
				$mfw_backup['soll_end_dtm']=$old['endzeit'];
				$mfw_backup['terminende']=$old['endzeit'];
			}
		}

		// Zeitraum formatieren
		if(!$db_start)
			$db_start=$mfw_backup['soll_beg_dtm'];
		if(!$db_start && $Termin_ID)
			$db_start=db_value("SELECT startzeit FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if(!$db_end)
			$db_end=$mfw_backup['soll_end_dtm'];
		if(!$db_end && $Termin_ID)
			$db_end=db_value("SELECT endzeit FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		$timespan=get_entry($db_start, 'datetime');
		if($db_end != $db_start)
			$timespan .= '-'.get_entry($db_end, 'zeit');

		// Terminart
		$apptype=0;
		if(!$apptype && fxIsArray($dbwerte) && isset($dbwerte['terminart']))
			$apptype=(int)$dbwerte['terminart'];
		if(!$apptype && fxIsArray($mfw_backup) && isset($mfw_backup['terminart']))
			$apptype=(int)$mfw_backup['terminart'];
		if(!$apptype)
			$apptype=FXP_AT_VARIOUS;

		// Beschreibung
		if(fxIsArray($dbwerte) && isset($dbwerte['beschreibung']))
			$beschreibung=$dbwerte['beschreibung'];
		else if(fxIsArray($mfw_backup) && isset($mfw_backup['beschreibung']) && strlen((string)$mfw_backup['beschreibung']))
			$beschreibung="'".convert_string($mfw_backup['beschreibung'], 'todb')."'";
		else
			$beschreibung="NULL";
		$adesc=dbValue2HTML($beschreibung);

		// Treffpunkt
		if(fxIsArray($dbwerte) && isset($dbwerte['treffpunkt']))
			$treffpunkt=$dbwerte['treffpunkt'];
		else if(fxIsArray($mfw_backup) && isset($mfw_backup['treffpunkt']) && strlen((string)$mfw_backup['treffpunkt']))
			$treffpunkt="'".convert_string($mfw_backup['treffpunkt'], 'todb')."'";
		else
			$treffpunkt="NULL";
		$aloc=dbValue2HTML($treffpunkt);

		// Name Termin
		if(fxIsArray($dbwerte) && isset($dbwerte['name_termin']))
			$name_termin=$dbwerte['name_termin'];
		else if(fxIsArray($mfw_backup) && isset($mfw_backup['name_termin']) && strlen((string)$mfw_backup['name_termin']))
			$name_termin="'".convert_string($mfw_backup['name_termin'], 'todb')."'";
		else
			$name_termin="NULL";
		$aref=dbValue2HTML($name_termin);

		// Outlook
		if($in_outlook)
			$in_outlook=1;
		else
			$in_outlook='NULL';

		$projekt_id=$sel_projekt_id;

		// Anzeigen,Anlegen,Ändern,Löschen
		$aaal=explode(',', meldung(16));
		$mode=$aaal[$GLOBALS['fxpglobals']['action']-1];

		// Personendaten
		if($applicant && !fxIsArray($pdata))
			$pdata=get_persondata($applicant);
		if(isset($_POST['daten']) && is_array($_POST['daten']))
		{
			reset($_POST['daten']);
			$akey=key($_POST['daten']);
		}
		else
			$akey=$GLOBALS['_maskcounter'];

		$contact=0;

		// Terminprojekt-Ressourcen
		$tres=array();
		if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
		{
			// Ansprechpartner immer
			$tres[$applicant]=true;
			if(isset($_POST['daten']) && is_array($_POST['daten']) && (($apptype == FXP_AT_INVITATION) || is_array($_POST['einladen'])))
			{
				$inv=array();
				$anspers=$applicant.'|';
				$anslen=strlen((string)$anspers);
				foreach($_POST['daten'][$akey] as $nr => $pv)
				{
					if(substr((string)$pv, 0, $anslen) == $anspers)
					{
						if(($GLOBALS['fxpglobals']['action'] != FXP_DELETE) && !$_POST['einladen'][$akey][$nr] && !$_POST['Person_eingeladen'][$akey][$nr])
							$inv[$nr]=1;
					}
				}
				foreach($_POST['daten'][$akey] as $nr => $pv)
				{
					if($_POST['einladen'][$akey][$nr] || $inv[$nr] || $_POST['Person_eingeladen'][$akey][$nr])
					{
						$pe=explode('|', $pv);
						$pid=$pe[0];
					}
					else
						continue;

					if($pid == $applicant)
						continue;

					if($_POST['einladen'][$akey][$nr])
						$tres[$pid]=true;
				}
			}
		}
		if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
			fxDebug($tres, '$tres');

		// Vorgangsnummer-Puffer
		$tvn_buffer=array();

		switch($GLOBALS['fxpglobals']['action'])
		{
			case FXP_CREATE:
				$Termin_ID=id_max('termine', 'termin_id');
				$MasterTermin_ID=$Termin_ID;

				$erl='NULL';	// erledigt
				$hbs='NULL';	// hat_bestaetigt
				$bta='NULL';	// bestaetigt_am
				$bam='NULL';	// bestaetigung_anmerk

				// Invitation to creator - 1770: Invitation created and accepted by $var1 on $var2 at $var3 o'clock
				if(fxIsArray($_POST['daten']) && (($apptype == FXP_AT_INVITATION) || fxIsArray($_POST['einladen'])) && ($applicant == $sender))
				{
					$hbs=$applicant;
					$bta="'".$GLOBALS['datetime']."'";
					$bam="'1-".convert_string(meldung(1770, false, '<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit')), 'todb')."'";
				}
//fxDebug(array('$hbs'=>$hbs, '$bta'=>$bta, '$bam'=>$bam),'Invitation', 0);

				// Neuer Zeiterfassungstermin auf Projekt
				if(($zeit_id == -1) && ($sel_projekt_id > 0))
				{
					$tname  = $timespan.': '.meldung($apptype, false);
					$tname .= ' ('.substr((string)$_POST['Name_Termin'], 0, 197-strlen((string)$tname)).')';
					if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
						echo($GLOBALS['fxpglobals']['action'].': <b>Neuer Zeiterfassungstermin auf Projekt-ID '.$sel_projekt_id.'</b> mit Start='.$db_start.' + Ende='.$db_end.'<br />');
					$projekt_id=app_prj(0, $sel_projekt_id, $Termin_ID, $tname, $db_start, $db_end, $tres);
//fxDebug($projekt_id,'$projekt_id', 0);
					if(!$projekt_id)
						$err=true;
				}

				if(!$err)
				{
					$sql  = "INSERT INTO termine";
					$sql .= " (mandanten_id, zeit_id, personen_id, ansprechpartner, person_angelegt, termin_id,";
					$sql .= " mastertermin_id, projekt_id, startzeit, endzeit, soll_st_tag, soll_beginn, beschreibung, terminart, treffpunkt,";
					$sql .= " wiederholung, endedatum_db, erledigt, urlaub, hat_bestaetigt, bestaetigt_am,";
					$sql .= " bestaetigung_anmerk, wiedervorlagedb, name_termin, aktiv_kz, in_outlook, verschieben, archiv)";
					$sql .= " VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$zeit_id.", ".$applicant.", 0, ".$sender.", ".$Termin_ID.",";
					$sql .= " ".$MasterTermin_ID.", ".$projekt_id.", '".$db_start."', '".$db_end."', ".$pdata['sollstunden_tag'].", '".$pdata['soll_beginn']."', ".$beschreibung.", ".$apptype.", ".$treffpunkt.",";
					$sql .= " 0, NULL, ".$erl.", 0, ".$hbs.", ".$bta.", ";
					$sql .= " ".$bam.", ".$dbwerte['wiedervorlage'].", ".$name_termin.", NULL, ".$in_outlook.", NULL, NULL)";
					if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
						fxDebug($sql, __LINE__.': $sql');
					$err=db_x($sql, 0);
//fxDebug($err,$sql, 0);
				}

				// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
				if($err)
				{
					error_msg(179, false, $mode, '<b>'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
					break;
				}
				else
				{
					$inscnt++;
					$changeinv=true;
				}
			break;

			case 3:	// Ändern
				$elt_sel_projekt_id=0;
				$changeinv=true;
				$changeit=true;
				if(fxIsArray($old))
				{
					$changeinv=false;
					$changeit=false;

					$ca=array(
						'*ipersonen_id'=>$applicant,
						'*iperson_angelegt'=>$sender,
						'*dstartzeit'=>$db_start,
						'*dendzeit'=>$db_end,
						'*iterminart'=>$apptype,
						'*ttreffpunkt'=>$treffpunkt,
						'*tname_termin'=>$name_termin,

						'iprojekt_id'=>$projekt_id,
						'fsoll_st_tag'=>$pdata['sollstunden_tag'],
						'dsoll_beginn'=>$pdata['soll_beginn'],
						'tbeschreibung'=>$beschreibung,
						'dwiedervorlagedb'=>$dbwerte['wiedervorlage'],
						'ierledigt'=>$dbwerte['erledigt']
					);

					foreach($ca as $k => $v)
					{
						$ki=false;
						if(substr((string)$k,0,1) == '*')
						{
							$ki=true;
							$k=substr((string)$k,1);
						}
						$kt=substr((string)$k,0,1);
						$kc=substr((string)$k,1);

						if($v === 'NULL')
							$v='';
						else if($kt == 't')
							$v=dbValue2HTML($v);

						if($old[$kc] != $v)
						{
							$changeit=true;
							if($ki)
								$changeinv=true;
						}

						if($changeit)
							break;
					}
				}

				// Prüfen, ob eingeladene Personen verändert wurden
				if(!$changeinv && fxIsArray($_POST['Person_eingeladen']) && fxIsArray($_POST['Person_eingeladen'][$akey]))
				{
					foreach($_POST['Person_eingeladen'][$akey] as $nr => $ival)
					{
						$pstat=0;
						if($ival)
							$pstat=1;

						$istat=0;
						if(fxIsArray($_POST['einladen']) && fxIsArray($_POST['einladen'][$akey]) && isset($_POST['einladen'][$akey][$nr]) && $_POST['einladen'][$akey][$nr])
							$istat=1;

						if($pstat != $istat)
						{
							$changeit=true;
							$changeinv=true;
							break;
						}
					}
				}

				if($changeit)
				{
					// Zeiterfassungstermin-Aufgabe und evtl. Projekt löschen
					if(fxIsArray($old) && $old['zeit_id'] && ($old['projekt_id'] > 0))
					{
						$sql="SELECT elter FROM projekte WHERE projekt_id=".$sel_projekt_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$elt_sel_projekt_id=(int)db_value($sql);
						if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
							echo($GLOBALS['fxpglobals']['action'].': <b>DELETE</B> SELECTED: $zeit_id='.$old['zeit_id'].', $elt_sel_projekt_id='.$elt_sel_projekt_id.', $sel_projekt_id='.$sel_projekt_id.', $projekt_id='.$old['projekt_id'].'<br />');
						$err=app_prj($old['projekt_id'], 0, $Termin_ID);
					}

					// Prüfen, ob ein Erinnerungstermin zum aktuellen Termin existiert
					if(!$err)
					{
						$where=" WHERE mastertermin_id=".$Termin_ID." AND termin_id<>".$Termin_ID." AND terminart=".FXP_AT_REMINDER." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$sql="SELECT termin_id FROM termine".$where;
						$remtermin=(int)db_value($sql);
						if($remtermin)
						{
							$sql="DELETE FROM termine".$where;
							if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
								fxDebug($sql, __LINE__.': Erinnerungstermin löschen');
							$err=db_x($sql, 0);
							// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
							if($err)
								error_msg(179, false, $mode, '<b>'.$tarten[180].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
						}
					}

					// Prüfen, ob Einladungstermine zum aktuellen Mastertermin existieren
					if(!$err && $changeinv)
					{
						$where=" WHERE mastertermin_id=".$Termin_ID." AND termin_id<>".$Termin_ID." AND terminart=".FXP_AT_INVITATION." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$sql="SELECT termin_id FROM termine".$where;
						$invtermin=(int)db_value($sql);
						if($invtermin)
						{
							$sql="DELETE FROM termine".$where;
							if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
								fxDebug($sql, __LINE__.': Einladungstermine löschen');
							$err=db_x($sql, 0);
							// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
							if($err)
								error_msg(179, false, $mode, '<b>'.$tarten[FXP_AT_INVITATION].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
						}
					}

					// Prüfen, ob Wiederholungstermine zum aktuellen Mastertermin existieren
					if(!$err)
					{
						$where=" WHERE mastertermin_id=".$Termin_ID." AND termin_id<>".$Termin_ID." AND wiederholung>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$sql="SELECT termin_id FROM termine".$where;
						$wdhtermin=(int)db_value($sql);
						if($wdhtermin)
						{
							$sql="DELETE FROM termine".$where;
							if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
								fxDebug($sql, __LINE__.': Wiederholungstermine löschen');
							$err=db_x($sql, 0);
							// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
							if($err)
								error_msg(179, false, $mode, '<b>'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
						}
					}

					// Neuer Zeiterfassungstermin auf Projekt
					if(($zeit_id == -1) && ($sel_projekt_id > 0))
					{
						$sql="SELECT projekt_id FROM projekte WHERE projekt_id=".$sel_projekt_id." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$sel_exists=(int)db_value($sql);
						if(!$sel_exists)
							$sel_projekt_id=$elt_sel_projekt_id;
						$tname  = $timespan.': '.meldung($apptype, false);
						$tname .= ' ('.substr((string)$_POST['Name_Termin'], 0, 197-strlen((string)$tname)).')';
						if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
							echo($GLOBALS['fxpglobals']['action'].': <b>Neuer Zeiterfassungstermin auf Projekt-ID '.$sel_projekt_id.'</b> mit Start='.$db_start.' + Ende='.$db_end.'<br />');
						$projekt_id=app_prj(0, $sel_projekt_id, $Termin_ID, $tname, $db_start, $db_end, $tres);
//fxDebug($projekt_id,'$projekt_id', 0);
						if(!$projekt_id)
							$err=true;
					}

					if(!$err)
					{
						$sql  = "UPDATE termine SET";
						$sql .= " personen_id=".$applicant.", person_angelegt=".$sender.", projekt_id=".$projekt_id.",";
						$sql .= " startzeit='".$db_start."', endzeit='".$db_end."', soll_st_tag=".$pdata['sollstunden_tag'].", soll_beginn='".$pdata['soll_beginn']."', beschreibung=".$beschreibung.",";
						$sql .= " terminart=".$apptype.", treffpunkt=".$treffpunkt.", name_termin=".$name_termin.", wiedervorlagedb=".$dbwerte['wiedervorlage'];
						if(isset($dbwerte['erledigt']))
							$sql .= ", erledigt=".$dbwerte['erledigt'];
						$sql .= " WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
							fxDebug($sql, __LINE__.': $sql');
						$err=db_x($sql, 0);
						// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
						if($err)
							error_msg(179, false, $mode, '<b>'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
						else
							$updcnt++;
					}
				}
			break;

			case 4:	// Löschen
				if($apptype == FXP_AT_INVITATION)
				{
					$dbwerte['daten']='';
					$contact=db_value("SELECT ansprechpartner FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				}

				// Zeiterfassungstermin-Aufgabe und evtl. Projekt löschen
				if($zeit_id && ($sel_projekt_id > 0))
				{
					if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
						echo($GLOBALS['fxpglobals']['action'].': <b>DELETE</B>: $zeit_id='.$zeit_id.', $sel_projekt_id='.$sel_projekt_id.', $projekt_id='.$projekt_id.'<br />');
					$err=app_prj($projekt_id, 0, $Termin_ID);
				}

				// Wird kein Mastertermin gelöscht bei einer Einladung (FXP_AT_INVITATION), dann nur "Erledigt"-Kennzeichen setzen
				if(!$err)
				{
					if(($MasterTermin_ID != $Termin_ID) && ($apptype == FXP_AT_INVITATION))
					{
						// 196: Invitation refused by $var1 on $var2 at $var3 o'clock
						if(!isset($ko_itext))
							$ko_itext=meldung(196, false, '<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit'));

						$sql  = "UPDATE termine SET hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."', bestaetigung_anmerk='0-".convert_string($ko_itext, 'todb')."', erledigt=1";
						$sql .= " WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					}
					else
					{
						$sql  = "DELETE FROM termine WHERE mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$sql .= " AND (termin_id=".$Termin_ID." OR (mastertermin_id=".$Termin_ID." AND (terminart IN (".FXP_AT_REMINDER.",".FXP_AT_INVITATION.") OR wiederholung>0)))";
					}
					if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
						fxDebug($sql, __LINE__.': $sql');
					$err=db_x($sql, 0);
					// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
					if($err)
						error_msg(179, false, $mode, '<b>'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
					else
					{
						$delcnt++;
						$changeinv=true;
					}
				}

				// Keinen Erinnerungs- und Wiederholungstermin anlegen, da oben bereits mitgelöscht
				$dbwerte['wiedervorlage']='NULL';
				$dbwerte['wiederholung']=0;

				// Falls Haupttermin gelöscht wurde alle evtl. eingeladenen Personen wieder ausladen
				if(($apptype != FXP_AT_INVITATION) && fxIsArray($_POST['daten']))
				{
					foreach($_POST['daten'][$akey] as $nr => $pv)
					{
						if($_POST['Person_eingeladen'][$akey][$nr])
							$_POST['einladen'][$akey][$nr]='';
					}
				}
			break;
		}

		// Mastertermin-ID=Termin-ID
		$MasterTermin_ID=$Termin_ID;
		$Termin_ID_rem=$Termin_ID;

		// Wiederholungstermin anlegen
		$wdhcnt=0;
		$wdhdtn='';
		if(!$err && ($dbwerte['wiederholung'] > 0))
		{
			if($dbwerte['endedatum_wdh'] != 'NULL')
				$edate=substr((string)$dbwerte['endedatum_wdh'], 1, 8);
			else
				$edate='99999999';
			$mcnt=(int)$dbwerte['wiederholung_anz'];
			if(!$mcnt)
				$mcnt=0;

			$aj=(int)substr((string)$db_start, 0, 4);	// Jahr
			$am=(int)substr((string)$db_start, 4, 2);	// Monat
			$at=(int)substr((string)$db_start, 6, 2);	// Tag

			$mo_tage=array(1 => 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31);
			// Auf Schaltjahr prüfen
			$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $aj));
			if($lj)
				$mo_tage[2]=29;

			$cnt=0;

			while(true)
			{
				$nt=$at;
				// täglich (665) oder wöchentlich (666)
				if(($dbwerte['wiederholung'] == 665) || ($dbwerte['wiederholung'] == 666))
				{
					if($dbwerte['wiederholung'] == 665)
						$nt++;
					else
						$nt += 7;
					if($nt > $mo_tage[$am])
					{
						$nt -= $mo_tage[$am];
						$am++;
						if($am > 12)
						{
							$am=1;
							$aj++;
							$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $aj));
							if($lj)
								$mo_tage[2]=29;
						}
					}
					$at=$nt;
				}
				// monatlich (667)
				else if($dbwerte['wiederholung'] == 667)
				{
					$am++;
					if($am > 12)
					{
						$am=1;
						$aj++;
						$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $aj));
						if($lj)
							$mo_tage[2]=29;
					}
					if($nt > $mo_tage[$am])
						$nt=$mo_tage[$am];
				}
				// jährlich
				else
				{
					$aj++;
					$lj=fx_date('L', fx_mktime(0, 0, 0, 1, 1, $aj));
					if($lj)
						$mo_tage[2]=29;
					if($nt > $mo_tage[$am])
						$nt=$mo_tage[$am];
				}

				$cnt++;
				$adate=$aj.substr('00'.$am, -2).substr('00'.$nt, -2);

				if(($edate != '99999999') && ($edate < $adate))
				{
					if($GLOBALS['__DGBLVL'] == 4)
						echo('<B>ABBRUCH:</B> EndDatum ('.$edate.') < AktDatum ('.$adate.')<br />');
					break;
				}
				if($mcnt && ($cnt > $mcnt))
				{
					if($GLOBALS['__DGBLVL'] == 4)
						echo('<B>ABBRUCH:</B> Counter ('.$cnt.') > Maxwert ('.$mcnt.')<br />');
					break;
				}

				if($GLOBALS['__DGBLVL'] == 4)
					echo('-> WDH #'.$cnt.': '.$adate.'<br />');

				// Wiederholungstermin in DB eintragen
				$projekt_id_wdh=$projekt_id;
				$db_wdh_start=$adate.substr((string)$db_start, 8);
				$db_wdh_end=$adate.substr((string)$db_end, 8);
				$ntid=id_max('termine', 'termin_id');

				// Neuer Zeiterfassungstermin auf Projekt
				if(($zeit_id == -1) && ($sel_projekt_id > 0))
				{
					$rep_timespan=get_entry($db_wdh_start, 'datetime');
					if($db_wdh_end != $db_wdh_start)
						$rep_timespan .= get_entry($db_wdh_end, 'zeit');
					$tname  = $rep_timespan.': '.meldung($apptype, false);
					$tname .= ' ('.substr((string)$_POST['Name_Termin'], 0, 197-strlen((string)$tname)).')';
					if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
						echo($GLOBALS['fxpglobals']['action'].': <b>Neuer Zeiterfassungstermin für Wiederholungstermin auf Projekt-ID '.$sel_projekt_id.'</b> mit Start='.$db_wdh_start.' + Ende='.$db_wdh_end.'<br />');
					$projekt_id_wdh=app_prj(0, $sel_projekt_id, $ntid, $tname, $db_wdh_start, $db_wdh_end, $tres);
					if(!$projekt_id_wdh)
					{
						$err=true;
						break;
					}
				}

				$sql  = "INSERT INTO termine";
				$sql .= " (mandanten_id, zeit_id, personen_id, ansprechpartner, person_angelegt, termin_id,";
				$sql .= " mastertermin_id, projekt_id, startzeit, endzeit, soll_st_tag, soll_beginn, beschreibung, terminart, treffpunkt,";
				$sql .= " wiederholung, endedatum_db, erledigt, urlaub, hat_bestaetigt, bestaetigt_am,";
				$sql .= " bestaetigung_anmerk, wiedervorlagedb, name_termin, aktiv_kz, in_outlook, verschieben, archiv)";
				$sql .= " VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$zeit_id.", ".$applicant.", 0, ".$sender.", ".$ntid.",";
				$sql .= " ".$MasterTermin_ID.", ".$projekt_id_wdh.", '".$db_wdh_start."', '".$db_wdh_end."', ".$pdata['sollstunden_tag'].", '".$pdata['soll_beginn']."', ".$beschreibung.", ".$apptype.", ".$treffpunkt.",";
				$sql .= " 0, NULL, NULL, 0, NULL, NULL, ";
				$sql .= " 'R".$cnt."', NULL, ".$name_termin.", NULL, ".$in_outlook.", NULL, NULL)";
				if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
					fxDebug($sql, __LINE__.': $sql');
				$err=db_x($sql, 0);
				// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
				if($err)
				{
					error_msg(179, false, $aaal[1], '<b>[R]-'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.get_entry($adate, 'datum').'</i>');
					break;
				}
				else
					$wdhdtn .= ', '.get_entry($adate, 'datum');
			}

			// Wiederholung + Datum updaten
			if(!$err && $cnt)
			{
				$sql  = "UPDATE termine SET wiederholung=".$dbwerte['wiederholung'].", endedatum_db='".$edate.substr('000000'.$mcnt, -6)."'";
				$sql .= " WHERE mastertermin_id=".$MasterTermin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
					fxDebug($sql, __LINE__.': $sql');
				$err=db_x($sql, 0);
				// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
				if($err)
					error_msg(179, false, $aaal[2], '<b>[R]-'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.$timespan.'</i>');
				else
					$wdhcnt=$cnt-1;
			}
		}

		// Erinnerungstermin anlegen
		if(!$err && ($dbwerte['wiedervorlage'] != 'NULL'))
		{
			$ntid=id_max('termine', 'termin_id');
			$erdtm=substr((string)$dbwerte['wiedervorlage'], 0, 9).substr((string)$db_start, 8, 6)."'";
			if($beschreibung == "NULL")
				$er_beschr="'";
			else
				$er_beschr=convert_string("<HR>", 'todb').substr((string)$beschreibung, 1);

			$sql  = "INSERT INTO termine";
			$sql .= " (mandanten_id, zeit_id, personen_id, ansprechpartner, person_angelegt, termin_id,";
			$sql .= " mastertermin_id, projekt_id, startzeit, endzeit, soll_st_tag, soll_beginn, beschreibung, terminart, treffpunkt,";
			$sql .= " wiederholung, endedatum_db, erledigt, urlaub, hat_bestaetigt, bestaetigt_am,";
			$sql .= " bestaetigung_anmerk, wiedervorlagedb, name_termin, aktiv_kz, in_outlook, verschieben, archiv)";
			$sql .= " VALUES";
			$sql .= " (".$GLOBALS['fxpglobals']['client'].", NULL, ".$applicant.", 0, ".$sender.", ".$ntid.",";
			$sql .= " ".$MasterTermin_ID.", ".$projekt_id.", ".$erdtm.", ".$erdtm.", ".$pdata['sollstunden_tag'].", '".$pdata['soll_beginn']."', '".convert_string('-> '.$timespan, 'todb').$er_beschr.", ".FXP_AT_REMINDER.", ".$treffpunkt.",";
			$sql .= " 0, NULL, NULL, 0, NULL, NULL, ";
			$sql .= " NULL, NULL, '".$tarten[FXP_AT_REMINDER].": ".substr((string)$name_termin, 1).", NULL, ".$in_outlook.", NULL, NULL)";
			if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
				fxDebug($sql, __LINE__.': $sql');
			$err=db_x($sql, 0);
//fxDebug($err,'Reminder: '.$sql, 0);
			// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
			if($err)
			{
				error_msg(179, false, $aaal[1], '<b>'.$tarten[180].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)', '<i>'.get_entry(substr((string)$erdtm, 1, 14), 'datetime').'</i>');
				break;
			}
		}

		// Einladungstermine anlegen
		$inv_list=array();
		if(!$err && $changeinv && fxIsArray($_POST['daten']) && (($apptype == FXP_AT_INVITATION) || fxIsArray($_POST['einladen'])))
		{
			// Ansprechpartner immer mit einladen
			$anspers=$applicant.'|';
			$anslen=strlen((string)$anspers);
			foreach($_POST['daten'][$akey] as $nr => $pv)
			{
				if(substr((string)$pv, 0, $anslen) == $anspers)
				{
					if(($GLOBALS['fxpglobals']['action'] != FXP_DELETE) && !$_POST['einladen'][$akey][$nr] && !$_POST['Person_eingeladen'][$akey][$nr])
						$_POST['einladen'][$akey][$nr]=1;
				}
			}
			foreach($_POST['daten'][$akey] as $nr => $pv)
			{
				if($_POST['einladen'][$akey][$nr] || $_POST['Person_eingeladen'][$akey][$nr])
				{
					$pe=explode('|', $pv);
					$pid=$pe[0];
					// Personendaten
					$ipdata=get_persondata($pid);
					$ifm_person=$ipdata['name'];
				}
				else
					continue;

				// Nie an die Hauptperson selbst, da automatisch eingeladen
				if($pid == $applicant)
				{
					if($GLOBALS['__DGBLVL'] == 4)
						echo($ifm_person.' ('.$pid.') ist Hauptperson<br />');
					continue;
				}

				// Einladen
				if($_POST['einladen'][$akey][$nr])
				{
					$ntid=id_max('termine', 'termin_id');

					// --- sollstunden/tag + soll_beginn für jede Person ermitteln
					$sql  = "INSERT INTO termine";
					$sql .= " (mandanten_id, zeit_id, personen_id, ansprechpartner, person_angelegt, termin_id,";
					$sql .= " mastertermin_id, projekt_id, startzeit, endzeit, soll_st_tag, soll_beginn, beschreibung, terminart, treffpunkt,";
					$sql .= " wiederholung, endedatum_db, erledigt, urlaub, hat_bestaetigt, bestaetigt_am,";
					$sql .= " bestaetigung_anmerk, wiedervorlagedb, name_termin, aktiv_kz, in_outlook, verschieben, archiv)";
					$sql .= " VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$zeit_id.", ".$pid.", ".$applicant.", ".$sender.", ".$ntid.",";
					$sql .= " ".$MasterTermin_ID.", ".$projekt_id.", '".$db_start."', '".$db_end."', ".$ipdata['sollstunden_tag'].", '".$ipdata['soll_beginn']."', ".$beschreibung.", ".FXP_AT_INVITATION.", ".$treffpunkt.",";
					$sql .= " 0, NULL, NULL, 0, NULL, NULL, ";
					$sql .= " 'E".$apptype."', NULL, ".$name_termin.", NULL, ".$in_outlook.", NULL, NULL)";
					if(($GLOBALS['__DGBLVL'] > 2) && ($GLOBALS['__DGBLVL'] < 5))
						fxDebug($sql, __LINE__.': $sql');
					$err=db_x($sql, 0);
//fxDebug($err,'Invitation: '.$sql, 0);
					// 179: Fehler beim $var1 der Terminart "$var2" für $var3 im Zeitraum $var4!
					if($err)
					{
						error_msg(179, false, $aaal[1], '<b>'.$tarten[FXP_AT_INVITATION].'</b>', $ifm_person, '<i>'.$timespan.'</i>');
						break;
					}

					if($GLOBALS['__DGBLVL'] == 4)
					{
						if($_POST['Person_eingeladen'][$akey][$nr])
							echo($ifm_person.' ('.$pid.') war bereits eingeladen - erneut einladen mit Änderungen<br />');
						else
							echo($ifm_person.' ('.$pid.') neu einladen<br />');
					}

					$inv_list[$pid]=array(
						'md'		=> 1,
						'dt'		=> $ipdata
					);
				}
				// Ausladen (DB-Zugriffe bereits oben)
				else if($pid != $applicant)
				{
					$inv_list[$pid]=array(
						'md'		=> 0,
						'dt'		=> $ipdata
					);
					if($GLOBALS['__DGBLVL'] == 4)
						echo($ifm_person.' ('.$pid.') ausladen<br />');
				}
			}
		}
//fxDebug($inv_list,'$inv_list');

		// E-Mails
		// ...send to applicant (=recipient)?
		if(!$err && check_mail(1735,'MA') && ($applicant != $sender) && fxIsArray($GLOBALS['_ua'][$applicant]) && strlen((string)$GLOBALS['_ua'][$applicant]['email']))
		{
			$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$applicant]['settings'];
			$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
			$lang=$GLOBALS['fxpglobals']['lang'];

			if(!isset($GLOBALS['_ma']['1736_'.$lang]))
				$GLOBALS['_ma']['1736_'.$lang]=explode('^', meldung(1736));	// New appointment^Changed appointment^Deleted appointment

			// Person|Terminart|Datum|Uhrzeit|Betreff|Treffpunkt|Beschreibung
			// Person|App. Type|Date|Time|Reference|Meeting Loc.|Description
			$md='email_app';
			if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
				$GLOBALS['_ma'][$md.'_'.$lang]=get_message($md);

			$atype=meldung($apptype,false);

			$adate=get_entry($db_start,'datum');
			$atime1=get_entry($db_start,'zeit');
			$atime2=get_entry($db_end,'zeit');
			$atimespan=$adate.', '.$atime1;

			$atime=$atime1;
			if($atime2 != $atime1)
			{
				$atimespan .= '-'.$atime2;
				$atime .= '-'.$atime2;
			}

			if(fxIsArray($old))
			{
				$htype=meldung($old['terminart'],false);

				$hdate=get_entry($old['startzeit'],'datum');
				$htime1=get_entry($old['startzeit'],'zeit');
				$htime2=get_entry($old['endzeit'],'zeit');

				$href=$old['name_termin'];
				$hloc=$old['treffpunkt'];
				$hdesc=$old['beschreibung'];

				$htime=$htime1;
				if($htime2 != $htime1)
					$htime .= '-'.$htime2;
			}
			else
			{
				$htype=$atype;

				$hdate=$adate;
				$htime1=$atime1;
				$htime2=$atime2;

				$href=$aref;
				$hloc=$aloc;
				$hdesc=$adesc;

				$htime=$atime;
			}

			// Data
			// ...Person
			$data  = 'N~';
			$data .= $applicant_name;
			// ...App. Type
			$data .= '|N~';
			if($htype != $atype)
				$data .= '<font style="color:#'.colorCode('grey').';">'.$htype.'</font>'.$GLOBALS['_carrow'].'<b>'.$atype.'</b>';
			else
				$data .= $atype;
			// ...Date
			$data .= '|N~';
			if($hdate != $adate)
				$data .= '<font style="color:#'.colorCode('grey').';">'.$hdate.'</font>'.$GLOBALS['_carrow'].'<b>'.$adate.'</b>';
			else
				$data .= $adate;
			// ...Time
			$data .= '|N~';
			if($htime != $atime)
				$data .= '<font style="color:#'.colorCode('grey').';">'.$htime.'</font>'.$GLOBALS['_carrow'].'<b>'.$atime.'</b>';
			else
				$data .= $atime;
			// ...Reference
			$data .= '|N~';
			if($href != $aref)
				$data .= '<font style="color:#'.colorCode('grey').';">'.$href.'</font>'.$GLOBALS['_carrow'].'<b>'.$aref.'</b>';
			else
				$data .= $aref;
			// ...Meeting Loc.
			$data .= '|N~';
			if($hloc != $aloc)
				$data .= '<font style="color:#'.colorCode('grey').';">'.$hloc.'</font>'.$GLOBALS['_carrow'].'<b>'.$aloc.'</b>';
			else
				$data .= $aloc;
			// ...Description
			$data .= '|W100%~';
			if($hdesc != $adesc)
				$data .= '<font style="color:#'.colorCode('grey').';">'.$hdesc.'</font>'.$GLOBALS['_carrow'].'<b>'.$adesc.'</b>';
			else
				$data .= $adesc;

			// Title: 183=$var1 from $var2 in the time span $var3
			$title=$GLOBALS['_ma']['1736_'.$lang][$GLOBALS['fxpglobals']['action']-2].': '.meldung(183,false, $atype, $applicant_name, $atimespan);

			// E-mail content: Edited by
			$content='<table width="80%" cellpadding="8" cellspacing="0" style="border:1px solid #'.colorCode('red').';border-radius:4px;box-shadow:2px 2px 4px #'.colorCode('darkgrey').';"><tr><td>'.meldung(47,false).': <b>'.$sender_name.'</b></td></tr></table><br /><br /><br />';
			if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
				$content .= mailDataTable($GLOBALS['_ma'][$md.'_'.$lang], array(0=>$data), $GLOBALS['_ma']['1736_'.$lang][0], colorCode('green'));
			else if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
				$content .= mailDataTable($GLOBALS['_ma'][$md.'_'.$lang], array(0=>$data), $GLOBALS['_ma']['1736_'.$lang][1], colorCode('blue'));
			else
				$content .= mailDataTable($GLOBALS['_ma'][$md.'_'.$lang], array(0=>$data), $GLOBALS['_ma']['1736_'.$lang][2], colorCode('red'));

			$merr=send_mail($applicant, $title, $content);

			$GLOBALS['fxpglobals']['settings']=$settings_backup;
			$GLOBALS['fxpglobals']['lang']=$lang_backup;

			if($merr)
			{
				// 61: An e-mail regarding $var1 could not be sent to $var2!
				error_msg(61, false, '<b>'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)');
			}
			else
			{
				// 62: An e-mail regarding $var1 has been successfully sent to $var2.
				error_msg(62, false, '<b>'.$tarten[$apptype].'</b>', '(<b style="color:#'.colorCode('darkgrey').';">'.$applicant_name.'</b>)');
			}
		}

		// ...send invitations/disinvitations?
		if(!$err && check_mail(1734,'MA') && fxIsArray($inv_list) && ($apptype != FXP_AT_INVITATION))
		{
//fxDebug($inv_list, 'Invitation/Disinvitations');

			// Cycle through all persons
			foreach($inv_list as $pid => $il)
			{
				if($il['md'] == 1)	// Invitation
					$mds='inv';
				else				// Disinvitation
					$mds='disinv';
				$merr=sendInvitationMails($pid, $Termin_ID, $mds);
			}
		}

		// ...if invitation is deleted send refusal e-mail to contact person?
		if(!$err && check_mail(1734,'MA') && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && ($apptype == FXP_AT_INVITATION) && $contact)
		{
			$merr=sendInvitationMails($contact, $Termin_ID, 'inv_refuse');
		}

		if($err)
			break;
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

	// Erfolgsmeldungen
	if(!$err)
	{
		// 192: Der Wiederholungstermin wurde erfolgreich am $var1 angelegt.
		if($wdhcnt == 1)
			error_msg(192, false, '<i>'.substr((string)$wdhdtn, 2).'</i>');
		// 193: $var1 Wiederholungstermine wurden erfolgreich an folgenden Tagen angelegt: $var2
		else if($wdhcnt > 1)
			error_msg(193, false, '<b>'.$wdhcnt.'</b>', '<i>'.substr((string)$wdhdtn, 2).'</i>');

		if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && !$updcnt)
		{
			// Es wurden keine Änderungen durchgeführt.
			error_msg(160);
		}
		else if(($GLOBALS['fxpglobals']['action'] != FXP_DELETE) || ($delcnt < 2))
		{
			// 144/145/146: $var1 wurde erfolgreich angelegt/geändert/gelöscht.
			error_msg(142+$GLOBALS['fxpglobals']['action'], false, '<b>'.$tarten[$apptype].'</b> (<b style="color:#'.colorCode('grey').';">'.$applicant_name.'</b>)');
		}
		else
		{
			// Es wurden $var1 Datensätze erfolgreich gelöscht.
			error_msg(116, false, $delcnt);
		}

		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			$MasterTermin_ID=0;
			$Termin_ID=0;
			unset($_POST['einladen']);
		}

		if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
		{
			unset($maskenfeldwerte);
			$maskenfeldwerte['personen_id']=$applicant;
			$maskenfeldwerte['terminart']=$apptype;
			$maskenfeldwerte['soll_beg_dtm']=$db_end;
			$maskenfeldwerte['terminstart']=$db_end;
		}

		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		{
			$maske99_nv=true;
			$siid=0;
		}
	}
	else if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
	{
		$MasterTermin_ID=0;
		$Termin_ID=0;
	}
//$err=true;
}

if($dolist && !sizeof($tidarr))
{
	$err=true;
	error_msg(1642);	// Es wurden keine Einträge angehakt.
}
?>