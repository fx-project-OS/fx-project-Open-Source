<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : termine_abw_db.inc                                           //
// Version     : 21.2                                                         //
// Begin       : 2020-11-02                                                   //
// Last Change : 2020-11-02                                                   //
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
 * PF 222: Absence Planner
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$tidarr=array();
$dolist=false;
if(($lkey > 0) && ($Termin_ID == -1))
{
	$dolist=true;
	if(strlen($simg) && ($siid > 0))
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
	$GLOBALS['_ea']=array();
	$GLOBALS['_ua']=array();
	$GLOBALS['_ma']=array();

	$chgcnt=0;
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

		// Get supervisor data
		$supervisor=get_realelter($applicant);
		if($supervisor && !isset($GLOBALS['_ua'][$supervisor]))
		{
			$pedata=get_email_adress($supervisor,true,true);
			if(fxIsArray($pedata) && fxIsArray($pedata[$supervisor]))
				$GLOBALS['_ua'][$supervisor]=$pedata[$supervisor];
			else
				$GLOBALS['_ua'][$supervisor]=false;
		}
		if(fxIsArray($GLOBALS['_ua'][$supervisor]))
			$supervisor_name=$GLOBALS['_ua'][$supervisor]['name2'];
		else
			$supervisor_name='';
//fxDebug($GLOBALS['_ua'][$supervisor], '$supervisor='.$supervisor.' -- '.$supervisor_name);
//echo('$supervisor='.$supervisor.' -- '.$supervisor_name.'<hr />'.$nl);

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

			// ..._POST
			$_POST['Termin_ID']=$Termin_ID;

			// ...mfw_backup
			$mfw_backup=array();
			if(fxIsArray($old))
			{
				$mfw_backup['fehltagsart']=$old['terminart'];
				$mfw_backup['soll_beg_dtm']=$old['startzeit'];
				$mfw_backup['soll_end_dtm']=$old['endzeit'];
				$mfw_backup['stunden']=$old['stunden'];
				$mfw_backup['urlaub']=$old['urlaub'];
			}
		}

		// E-Mail
		$sendmail=false;

		// Urlaubstage
		$vacdiff=0.00;

		// Name der Person ermitteln
		if(!fxIsArray($pdata))
			$pdata=get_persondata($applicant);
//fxDebug($pdata, '$pdata: '.$applicant);

		// Zeitraum formatieren
		if(!$db_start && fxIsArray($old))
		{
			$db_start=$old['startzeit'];
			$db_end=$old['endzeit'];
		}
		$startdate=get_entry($db_start,'datetime');
		$enddate=get_entry($db_end,'datetime');
		$timespan=$startdate.' - '.$enddate;

		// Tagesanzahl
		if(fxIsArray($old) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			$days=$old['urlaub'];
		else if($awtage)
			$days=$awtage;
		else
			$days=$mfw_backup['urlaub'];

		// Stunden
		if(fxIsArray($old) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			$hours=$old['stunden'];
		else if($db_stunden)
			$hours=$db_stunden;
		else
			$hours=$mfw_backup['stunden'];

		// Fehltagsart
		if(fxIsArray($old) && ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
			$ftart=$old['terminart'];
		else if($dbwerte['fehltagsart'])
			$ftart=(int)$dbwerte['fehltagsart'];
		else
			$ftart=(int)$mfw_backup['fehltagsart'];
		$ftnew=getAcceptedApplicationID($ftart);
//fxDebug($ftart,'$ftart');

		// Is the absence appointment a request?
		$request=0;
		if($ftnew != $ftart)
			$request=$ftart;

		// Accept requests if the sender is also the supervisor (except if request should be deleted)
		if($request && $supervisor && ($supervisor == $sender) && ($GLOBALS['fxpglobals']['action'] != FXP_DELETE))
		{
			$ftart=$ftnew;
			$request=0;
			$supervisor=0;
		}

		// Outlook
		if($in_outlook)
			$in_outlook=1;
		else
			$in_outlook='NULL';

		// $mode: Display, Create, Edit or Delete
		$dced=explode(',', meldung(16));
		$mode=$dced[$GLOBALS['fxpglobals']['action']-1];

		switch($GLOBALS['fxpglobals']['action'])
		{
			case FXP_CREATE:
//fxDebug($satage, '$satage');
				foreach($satage as $sadatum => $sadauer)
				{
					// Gleittag(e)
					if((($ftart == FXP_AT_SLIDINGDAYREQUEST) || ($ftart == FXP_AT_SLIDINGDAY)) && ($sadauer > 0.00))
					{
						$db_start=$sadatum.$db_startzeit;
						$db_end=$sadatum.$db_endzeit;
						$awtage=$sadauer;
					}

					// Bestätigungen
					$hat_bestaetigt='NULL';
					$bestaetigt_am='NULL';
					$bestaetigung_anmerk='NULL';
					$erledigt=0;
					if(!$request)
					{
						// 83: Accepted and confirmed by $var1 on $var2 at $var3 o'clock
						$ok_text=meldung(83,false,'<b>'.$GLOBALS['_ua'][$sender]['name2'].'</b>',get_entry($GLOBALS['datetime'], 'datum'),get_entry($GLOBALS['datetime'],'zeit'));
						$hat_bestaetigt=$sender;
						$bestaetigt_am="'".$GLOBALS['datetime']."'";
						$bestaetigung_anmerk="'1-".convert_string($ok_text,'todb')."'";
						$erledigt=1;
					}

					$ntid=id_max('termine', 'termin_id');
					$aid=$ntid;

					$sql ="INSERT INTO termine";
					$sql .= " (mandanten_id, zeit_id, personen_id, ansprechpartner, person_angelegt, termin_id,";
					$sql .= " mastertermin_id, projekt_id, startzeit, endzeit, soll_st_tag, soll_beginn, beschreibung, terminart, treffpunkt,";
					$sql .= " wiederholung, endedatum_db, erledigt, urlaub, stunden, hat_bestaetigt, bestaetigt_am,";
					$sql .= " bestaetigung_anmerk, wiedervorlagedb, name_termin, aktiv_kz, in_outlook, verschieben, archiv)";
					$sql .= " VALUES";
					$sql .= " (".$GLOBALS['fxpglobals']['client'].", NULL, ".$applicant.", 0, ".$sender.", ".$ntid.",";
					$sql .= " ".$ntid.", 0, '".$db_start."', '".$db_end."', ".$pdata['sollstunden_tag'].", '".$pdata['soll_beginn']."', ".$dbwerte['beschreibung'].", ".$ftart.", NULL,";
					$sql .= " 0, NULL, ".$erledigt.", ".$awtage.", ".$db_stunden.", ".$hat_bestaetigt.", ".$bestaetigt_am.", ";
					$sql .= " ".$bestaetigung_anmerk.", NULL, '".convert_string($tarten[$ftart], 'todb')."', NULL, ".$in_outlook.", NULL, NULL)";
//fxDebug($sql, '$sql');
					$err=db_x($sql);
					// 478: Fehler beim $var1 von $var2 Tag(en) der Abwesenheitsart "$var3" für $var4 im Zeitraum $var5!
					if($err)
						error_msg(478, false, $mode, $days, '<b>'.$tarten[$ftart].'</b>', $applicant_name, $timespan);

					// Gleittag
					if(!$err && ($ftart == FXP_AT_SLIDINGDAY) && ($sadauer > 0.00))
					{
						$data_array=array(
							'personen_id'	=> $applicant,
							'ist_beginn'	=> $db_start,
							'ist_ende'		=> $db_end,
							'pause'			=> $pdata['pause'],
							'zeitaufwand'	=> (int)($pdata['sollstunden_tag']*3600*$sadauer)
						);

						// ...Halber Gleittag
						if($sadauer < 1.00)
						{
							$asc=validate_entry(get_entry($pdata['soll_beginn'], 'zeit'), $ierr, 'zeitspanne');
							$ast=($pdata['wochenarbeitstd']*3600)/$pdata['arbeitstage'];
							$ssc=$asc+($ast/2)+$pdata['pause'];
							$h=(int)($ssc/3600); $ssc=$ssc-$h*3600;
							$m=(int)($ssc/60);
							$s=$ssc-$m*60;

							$data_array['ist_beginn']=substr($data_array['ist_beginn'], 0, 8).substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
							$data_array['pause']=0;
							$data_array['zeitaufwand']=(int)($data_array['zeitaufwand']*$sadauer);
						}

						$err=gleittag($ntid, $data_array);
						if($err)
							error_msg($err, false);
					}

					if($err)
						break;
				}

				// Urlaubsdifferenz
				if($ftart == FXP_AT_VACATION)
					$vacdiff=$awtage;

				// Mails versenden
				$sendmail=true;
			break;

			case FXP_CHANGE:
				$aid=$Termin_ID;
				// Bestätigungen
				$add='';
				if(!$request)
				{
					// 83: Accepted and confirmed by $var1 on $var2 at $var3 o'clock
					$ok_text=meldung(83,false,'<b>'.$GLOBALS['_ua'][$sender]['name2'].'</b>',get_entry($GLOBALS['datetime'], 'datum'),get_entry($GLOBALS['datetime'],'zeit'));
					$add .= ", hat_bestaetigt=".$sender;
					$add .= ", bestaetigt_am='".$GLOBALS['datetime']."'";
					$add .= ", bestaetigung_anmerk='1-".convert_string($ok_text,'todb')."'";
					$add .= ", erledigt=1";
				}
				$sql ="UPDATE termine SET";
				$sql .= " personen_id=".$applicant.", person_angelegt=".$sender.",";
				$sql .= " startzeit='".$db_start."', endzeit='".$db_end."', soll_st_tag=".$pdata['sollstunden_tag'].", soll_beginn='".$pdata['soll_beginn']."', beschreibung=".$dbwerte['beschreibung'].",";
				$sql .= " terminart=".$ftart.", urlaub=".$awtage.", stunden=".$db_stunden.", name_termin='".convert_string($tarten[$ftart], 'todb')."', in_outlook=".$in_outlook;
				$sql .= $add;
				$sql .= " WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
//fxDebug($sql, '$sql');
				$err=db_x($sql);
				// 478: Fehler beim $var1 von $var2 Tag(en) der Abwesenheitsart "$var3" für $var4 im Zeitraum $var5!
				if($err)
					error_msg(478, false, $mode, $days, '<b>'.$tarten[$ftart].'</b>', $applicant_name, $timespan);

				// Alten Gleittag löschen
				if(!$err && fxIsArray($old) && ($old['terminart'] == FXP_AT_SLIDINGDAY))
				{
					$err=gleittag($Termin_ID, false);
					if($err)
						error_msg($err, false);
				}
				// Neuen Gleittag anlegen
				$sday=substr($db_start, 0, 8);
				$sadauer=0.00;
				if(isset($satage[$sday]))
					$sadauer=$satage[$sday];
				if(!$err && ($ftart == FXP_AT_SLIDINGDAY) && ($sadauer > 0.00))
				{
					$data_array=array(
						'personen_id'	=> $applicant,
						'ist_beginn'	=> $db_start,
						'ist_ende'		=> $db_end,
						'pause'			=> $pdata['pause'],
						'zeitaufwand'	=> (int)($pdata['sollstunden_tag']*3600*$sadauer)
					);

					// ...Halber Gleittag
					if($sadauer < 1.00)
					{
						$asc=validate_entry(get_entry($pdata['soll_beginn'], 'zeit'), $ierr, 'zeitspanne');
						$ast=($pdata['wochenarbeitstd']*3600)/$pdata['arbeitstage'];
						$ssc=$asc+($ast/2)+$pdata['pause'];
						$h=(int)($ssc/3600); $ssc=$ssc-$h*3600;
						$m=(int)($ssc/60);
						$s=$ssc-$m*60;

						$data_array['ist_beginn']=substr($data_array['ist_beginn'], 0, 8).substr('0'.$h, -2).substr('0'.$m, -2).substr('0'.$s, -2);
						$data_array['pause']=0;
						$data_array['zeitaufwand']=(int)($data_array['zeitaufwand']*$sadauer);
					}

					$err=gleittag($Termin_ID, $data_array);
					if($err)
						error_msg($err, false);
				}

				// Urlaubsdifferenz
				if($ftart == FXP_AT_VACATION)
					$vacdiff=$awtage;

				// E-Mail versenden, falls Zeitraum, Fehltagsart oder Person verändert wurde
				if(fxIsArray($old) && ($db_start != $old['startzeit']) || ($db_end != $old['endzeit']) || ($ftart != $old['terminart']) || ($applicant != $old['personen_id']))
					$sendmail=true;

				if(!$err)
					$chgcnt++;
			break;

			case FXP_DELETE:
				$aid=$Termin_ID;
				$sql="DELETE FROM termine WHERE termin_id=".$Termin_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
//fxDebug($sql, '$sql');
				$err=db_x($sql);
				// 478: Fehler beim $var1 von $var2 Tag(en) der Abwesenheitsart "$var3" für $var4 im Zeitraum $var5!
				if($err)
					error_msg(478, false, $mode, $days, '<b>'.$tarten[$old['terminart']].'</b>', $applicant_name, $timespan);

				// Gleittag
				if(!$err && fxIsArray($old) && ($old['terminart'] == FXP_AT_SLIDINGDAY))
				{
					$err=gleittag($Termin_ID, false);
					if($err)
						error_msg($err, false);
				}

				// Mails versenden
				$sendmail=true;

				if(!$err)
					$delcnt++;
			break;
		}

		// Zurücksetzen der bereits berechneten Urlaubstage vom "Urlaubskonto"
		if(!$err && (($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)) && fxIsArray($old) && $old['personen_id'] && ($old['terminart'] == FXP_AT_VACATION) && ($old['urlaub'] != 0.00))
		{
			if($old['startzeit'])
				$year=(int)substr($old['startzeit'],0,4);
			else
				$year=1970;
			$err=reset_resturlaub($old['personen_id'], $year);
			// 479: Fehler bei der Neukalkulation der Resturlaubstage für $var1!
			if($err)
			{
				$sql="SELECT pname, vorname FROM personen WHERE personen_id=".$old['personen_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$pnd=db_values($sql);
				$pnd_name=$pnd['pname'];
				if(strlen(trim($pnd['vorname'])))
					$pnd_name=trim($pnd['vorname']).' '.$pnd_name;
				error_msg(479, false,'<b>'.$pnd_name.'</b>');
			}
		}

		// Bilanzen löschen, damit diese beim Report wieder aktualisiert wird
		if(!$err && (($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)) && fxIsArray($old) && $old['personen_id'])
		{
//echo('1. bilanz_zeitkonto_loeschen('.$old['personen_id'].', '.$old['startzeit'].');<br />'.$nl);
			$err=bilanz_zeitkonto_loeschen($old['personen_id'], $old['startzeit']);
			if($err)
				error_msg(1868, false);	// Fehler beim Löschen veralteter Bilanzdatensätze!
		}
		if(!$err && ($GLOBALS['fxpglobals']['action'] != FXP_DELETE) && $applicant)
		{
//echo('2. bilanz_zeitkonto_loeschen('.$applicant.', '.$db_start.');<br />'.$nl);
			$err=bilanz_zeitkonto_loeschen($applicant, $db_start);
			if($err)
				error_msg(1868, false);	// Fehler beim Löschen veralteter Bilanzdatensätze!
		}

		if(!$err && $applicant && ($vacdiff != 0.00))
		{
			if($db_start)
				$year=(int)substr($db_start,0,4);
			else
				$year=1970;
			$err=reset_resturlaub($applicant, $year);
			// 479: Fehler bei der Neukalkulation der Resturlaubstage für $var1!
			if($err)
				error_msg(479, false,'<b>'.$applicant_name.'</b>');
		}

		// Create emails
		if(!$db_start)
		{
			$db_start=$mfw_backup['soll_beg_dtm'];
			$db_end=$mfw_backup['soll_end_dtm'];
			$dbwerte['personen_id']=$applicant;
		}
		$prpa=getMailPersons(array(), 1733, $applicant, false, $db_start, $db_end);
		if(!$err && $sendmail && sizeof($prpa))
		{
			$emsa=array();

			$adesc='';
			if(fxIsArray($dbwerte) && isset($dbwerte['beschreibung']))
				$adesc=dbValue2HTML($dbwerte['beschreibung']);
			else if(fxIsArray($old))
				$adesc=$old['beschreibung'];

			// Editor
			$editor='';
			if($applicant != $sender)
			{
				// 47: Edited by
				$editor='<table width="80%" cellpadding="8" cellspacing="0" style="border:1px solid #'.colorCode('red').';border-radius:4px;box-shadow:2px 2px 4px #'.colorCode('darkgrey').';"><tr><td>$var1: <b>'.$GLOBALS['_ua'][$sender]['name2'].'</b></td></tr></table>';
			}

			$overtime=false;
			if(($ftart == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($ftart == FXP_AT_OVERTIMEOUTPAY))
				$overtime=true;

			// Get tasks with effort>0 in which the applicant would be absent
			$mtasks=getAbsentTasks($applicant,$db_start,$db_end);

			$GLOBALS['fxpglobals']['settings']=$settings_backup;
			$GLOBALS['fxpglobals']['lang']=$lang_backup;

			// Send to supervisor?
			if($supervisor && isset($prpa[$supervisor]) && fxIsArray($GLOBALS['_ua'][$supervisor]) && strlen($GLOBALS['_ua'][$supervisor]['email']))
			{
				$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$supervisor]['settings'];
				$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
				$lang=$GLOBALS['fxpglobals']['lang'];

				// Create e-mail
				$with_tasks=false;
				if(!$overtime && ($GLOBALS['fxpglobals']['action'] != FXP_DELETE))
					$with_tasks=true;
				absentAppMail($supervisor, $with_tasks);

				$info=$editor;
				if(strlen($info))
					$info=strtr($info, array('$var1'=>meldung(47)));
				$GLOBALS['_ea']=array($supervisor=>array('email'=>$GLOBALS['_ua'][$supervisor]['email'], 'supervisor'=>array('title'=>$title, 'info'=>$info, 'ics'=>$aid, 'data'=>array($content))));
//fxDebug($GLOBALS['_ea'],'$GLOBALS[\'_ea\']');
				$sent=sendPersonMails($GLOBALS['_ea'],$GLOBALS['_ua']);

				$GLOBALS['fxpglobals']['settings']=$settings_backup;
				$GLOBALS['fxpglobals']['lang']=$lang_backup;

				if($sent == false)
				{
					// 60: An email regarding $var1 could not be sent to the supervisor ($var2)!
					error_msg(60, false, '<b>'.$tarten[$ftart].'</b>', '<b class=darkgrey>'.$GLOBALS['_ua'][$supervisor]['name2'].'</b>');
				}
				else
				{
					// 1157: An email regarding $var1 has been successfully sent to the supervisor ($var2).
					error_msg(1157, false, '<b>'.$tarten[$ftart].'</b>', '<b class=darkgrey>'.$GLOBALS['_ua'][$supervisor]['name2'].'</b>');
				}

				$emsa[$supervisor]=true;
			}

			// Send to applicant (=recipient)?
			if(strlen($editor) && check_mail(1733,'MA') && fxIsArray($GLOBALS['_ua'][$applicant]) && strlen($GLOBALS['_ua'][$applicant]['email']))
			{
				$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$applicant]['settings'];
				$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
				$lang=$GLOBALS['fxpglobals']['lang'];

				// Create e-mail
				absentAppMail($applicant, false);

				$info=$editor;
				if(strlen($info))
					$info=strtr($info, array('$var1'=>meldung(47)));
				$GLOBALS['_ea']=array($applicant=>array('email'=>$GLOBALS['_ua'][$applicant]['email'], 'applicant'=>array('title'=>$title, 'info'=>$info, 'data'=>array($content))));
				if(!$request)
					$GLOBALS['_ea']['applicant']['ics']=$aid;
//fxDebug($GLOBALS['_ea'],'$GLOBALS['_ea']');
				$sent=sendPersonMails($GLOBALS['_ea'],$GLOBALS['_ua']);

				$GLOBALS['fxpglobals']['settings']=$settings_backup;
				$GLOBALS['fxpglobals']['lang']=$lang_backup;

				if($sent == false)
				{
					// 61: An email regarding $var1 could not be sent to $var2!
					error_msg(61, false, '<b>'.$tarten[$ftart].'</b>', '<b class=darkgrey>'.$applicant_name.'</b>');
				}
				else
				{
					// 62: An email regarding $var1 has been successfully sent to $var2.
					error_msg(62, false, '<b>'.$tarten[$ftart].'</b>', '<b class=darkgrey>'.$applicant_name.'</b>');
				}

				$emsa[$applicant]=true;
			}

			// E-Mail to project leaders etc.
			if(sizeof($mtasks) && !$overtime)
			{
//fxDebug($prpa, '$prpa');
				foreach($prpa as $dpers => $dtype)
				{
					if(!isset($emsa[$dpers]))	// only if not already sent
					{
						if($dtype > 1)
							$mta=$mtasks;
						else
						{
							$mta=array();
							foreach($mtasks as $l => $td)
							{
								if(($td['projektmanager'] == $dpers) || ($td['projektleiter'] == $dpers) || ($td['vertreter'] == $dpers))
									$mta[]=$td;
							}
						}
//fxDebug($mta,'$mta: $dpers='.$dpers);

						if(sizeof($mta))
						{
							if(!isset($GLOBALS['_ua'][$dpers]))
							{
								$pedata=get_email_adress($dpers,true,true);
								if(fxIsArray($pedata) && fxIsArray($pedata[$dpers]))
									$GLOBALS['_ua'][$dpers]=$pedata[$dpers];
								else
									$GLOBALS['_ua'][$dpers]=false;
							}

							if(fxIsArray($GLOBALS['_ua'][$dpers]) && strlen($GLOBALS['_ua'][$dpers]['email']))
							{
								$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$dpers]['settings'];
								$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
								$lang=$GLOBALS['fxpglobals']['lang'];

								// Create e-mail
								absentAppMail($dpers, true);

								$info=$editor;
								if(strlen($info))
									$info=strtr($info, array('$var1'=>meldung(47)));
								$GLOBALS['_ea']=array($dpers=>array('email'=>$GLOBALS['_ua'][$dpers]['email'], 'pl'=>array('title'=>$title, 'info'=>$info, 'data'=>array($content))));
								if(!$request)
									$GLOBALS['_ea']['pl']['ics']=$aid;
//fxDebug($GLOBALS['_ea'],'$GLOBALS['_ea']');
								$sent=sendPersonMails($GLOBALS['_ea'],$GLOBALS['_ua']);

								$GLOBALS['fxpglobals']['settings']=$settings_backup;
								$GLOBALS['fxpglobals']['lang']=$lang_backup;

								if(!isset($ertype))
									$ertype=array();
								if(!isset($ertype[$GLOBALS['fxpglobals']['lang']]))
									$ertype[$GLOBALS['fxpglobals']['lang']]=explode('|',meldung(3126));	// project participant|resource planner|general manager|administrator

								if($sent == false)
								{
									// 63: An Email regarding $var1 could not be sent to the $var2 ($var3)!
									error_msg(63, false, '<b>'.$tarten[$ftart].'</b>', $ertype[$GLOBALS['fxpglobals']['lang']][$dtype-1], '<b class=darkgrey>'.$GLOBALS['_ua'][$dpers]['name2'].'</b>');
								}
								else
								{
									// 64: An Email regarding $var1 has been successfully sent to the $var2 ($var3).
									error_msg(64, false, '<b>'.$tarten[$ftart].'</b>', $ertype[$GLOBALS['fxpglobals']['lang']][$dtype-1], '<b class=darkgrey>'.$GLOBALS['_ua'][$dpers]['name2'].'</b>');
								}
							}
						}

						$emsa[$dpers]=true;
					}
				}
			}
		}

		if($err)
			break;
	}
//$err=true;

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;

	// Erfolgsmeldung 144/145/146: $var1 wurde erfolgreich angelegt/geändert/gelöscht.
	if(!$err)
	{
		if(($GLOBALS['fxpglobals']['action'] != FXP_DELETE) || ($delcnt < 2))
			error_msg(142+$GLOBALS['fxpglobals']['action'], false, '<b>'.$tarten[$ftart].'</b> ('.$applicant_name.')');
		else
			error_msg(116, false, $delcnt);	// Es wurden $var1 Datensätze erfolgreich gelöscht.

		if($GLOBALS['fxpglobals']['action'] != FXP_CHANGE)
		{
			$Termin_ID=0;
			if(isset($_POST['Soll_Beg_Dtm']))
				unset($_POST['Soll_Beg_Dtm']);
			if(isset($_POST['Soll_End_Dtm']))
				unset($_POST['Soll_End_Dtm']);
			$maskenfeldwerte['beschreibung']='';
		}

		if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
		{
			$maske453_nv=true;
			$siid=0;
		}
	}
	else if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
		$Termin_ID=0;
}

if($dolist && !sizeof($tidarr))
{
	$err=true;
	error_msg(1642);	// Es wurden keine Einträge angehakt.
}
?>