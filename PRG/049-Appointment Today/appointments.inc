<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : start_termine.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2020-11-04                                                   //
// Last Change : 2021-04-12                                                   //
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
 * PF 49: Appointment Today
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
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

	$GLOBALS['_maskcounter']=1;
	$tr=$GLOBALS['fxpglobals']['tr'];
}

// Sort
$timestamp='';
if(fxIsArray($_GET))
{
	if(!isset($_GET['Heute_a']) && !isset($_GET['Heute_e']))
	{
		if(isset($_GET['gj']))
		{
			$_GET['Heute_a']=$_GET['gj'].'0101000000';
			$_GET['Heute_e']=$_GET['gj'].'1231240000';
		}
		else if(isset($_GET['gm']))
		{
			$_GET['Heute_a']=substr($_GET['gm'],0,6).'01000000';
			$_GET['Heute_e']=substr(ultimo($_GET['Heute_a']),0,8).'240000';
		}
		else if(isset($_GET['gw']))
		{
			$_GET['Heute_a']=substr($_GET['gw'],0,6).substr($_GET['gw'],10,2).'000000';
			$_GET['Heute_e']=substr(time_add($_GET['Heute_a'], 6, 'D'),0,8).'240000';
		}
		else if(isset($_GET['gd']))
		{
			$_GET['Heute_a']=substr($_GET['gd'],0,8).'000000';
			$_GET['Heute_e']=substr($_GET['gd'],0,8).'240000';
		}
	}
}
if(!isset($sort_person) && fxIsArray($_GET) && isset($_GET['sp']))
{
	$sort_person=(int)$_GET['sp'];
	$Heute_a=$_GET['Heute_a'];
	$Heute_e=$_GET['Heute_e'];

	// Benutzereinstellungen anpassen
	$sval=1464;			// Ja:		Sortierung nach Personen
	if(!$sort_person)
		$sval=1465;		// Nein:	Sortierung nach Zeitraum

	$whr=" WHERE einstellungs_id=120 AND benutzer_id=".$GLOBALS['fxpglobals']['user']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$chk="SELECT einstellungswerte_id FROM s_einstellungswerte".$whr;
	$cid=(int)db_value($chk);
	if($cid)
		$eid=$cid;
	else
		$eid=id_max('s_einstellungswerte', 'einstellungswerte_id', '', '', 401);

	if(!$cid)	// INSERT
	{
		$sql  = "INSERT INTO s_einstellungswerte";
		$sql .= " (mandanten_id, benutzer_id, einstellungswerte_id, einstellungs_id, einstellungswert) ";
		$sql .= "VALUES";
		$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$GLOBALS['fxpglobals']['user'].", ".$eid.", 120, '".$sval."')";
	}
	else		// UPDATE
	{
		$sql  = "UPDATE s_einstellungswerte";
		$sql .= " SET einstellungswert='".$sval."'";
		$sql .= $whr;
	}
	$eerr=db_x($sql);
fxDebug($eerr, $sql, 2);

	$GLOBALS['fxpglobals']['settings']['k_sort_person']=meldung($sval,true);
	// Save globals
	fxSession('g','s');
}

// Appointments
setForm(true);

$index=true;
if($GLOBALS['fxpglobals']['tr'] == 49)
{
	if(fxIsArray($_GET) && isset($_GET['sp']))
		$mc=1;
	else
		$mc=2;
	$GLOBALS['_maskcounter']=$mc;
}
else
	$mc=$GLOBALS['_maskcounter'];

$Zeitraum_nw=true;
$Person_alias_nw=true;
$Zeit_nw=true;
$genehmigen_nw=true;
$no_person=0;

$glstyle='margin-top:3px;margin-left:5px;';
$gmstyle='margin-left:10px;';
$grstyle='margin-left:10px;';

// Benutzereinstellungen bzgl. Sortierreihenfolge ermitteln
$fcol='Zeitraum';
$lcol='erledigt';
if(strtolower(substr($GLOBALS['fxpglobals']['settings']['k_sort_person'], 0, 1)) == 'n')
{
	$sort_person=0;
	$Zeitraum_sort='DESC';
	$colspan=6;
}
else
{
	$sort_person=1;
	$colspan=5;
}
$fcoll=strtolower($fcol);
$lcoll=strtolower($lcol);
//echo('sort_person='.$sort_person.', $colspan='.$colspan.', $fcol='.$fcol.', $lcol='.$lcol.'<hr />'.$nl);

// Alle Grafiken in ein Array speichern -> Code übersichtlicher
$TerminGrafiken=array
(
	FXP_AT_REMINDER					=> 'c_rem',
	FXP_AT_INVITATION				=> 'c_inv',
	FXP_AT_MEETING					=> 'c_meet',
	FXP_AT_CONFERENCE				=> 'c_conf',
	FXP_AT_BUSINESSDINNER			=> 'c_eat',
	FXP_AT_CONTACT					=> 'c_cont',
	FXP_AT_MESSAGE					=> 'c_msg',
	FXP_AT_PROJECTMESSAGE			=> 'c_pmsg',
	FXP_AT_PRIVATE					=> 'c_priv',
	FXP_AT_TODO						=> 'c_todo',

	FXP_AT_ABSENT					=> 'c_abs',
	FXP_AT_VACATION					=> 'c_hday',
	FXP_AT_VACATIONREQUEST			=> 'c_hday',
	FXP_AT_SPECIALVACATION			=> 'c_shday',
	FXP_AT_SPECIALVACATIONREQUEST	=> 'c_shday',
	FXP_AT_SICK						=> 'c_sick',
	FXP_AT_SICKCALL					=> 'c_sick',
	FXP_AT_TRAINING					=> 'c_trn',
	FXP_AT_TRAININGREQUEST			=> 'c_trn',
	FXP_AT_SLIDINGDAY				=> 'c_flex',
	FXP_AT_SLIDINGDAYREQUEST		=> 'c_flex',
	FXP_AT_OVERTIMEOUTPAY			=> 'c_otpay',
	FXP_AT_OVERTIMEOUTPAYREQUEST	=> 'c_otpay',

	FXP_AT_MILESTONE				=> 'psml'
);

$text_38=get_text(38, '15');		// Beschr.
$text_66=get_text(66, '15');		// Vorgang
$text_349=get_text(349, '10');		// Ansprechp.
$text_434=get_text(434, '15');		// Terminart
$text_541=get_text(541, '15');		// Treffpunkt
$text_1092=get_text(1092, '15');	// Betreff
$text_1325=get_text(1325, '15');	// Zeitraum
$text_1370=get_text(1370, '15');	// Anmerkung
$text_1877=get_text(1877, '15');	// Stunden

// Wochentags-Namen
$wochentage=fxf_getWeekdayNames(2);

// Datum ermitteln
$today=substr($GLOBALS['datetime'],0,8).'000000';
if(isset($_GET) && isset($_GET['Heute_a']))
	$Heute_a=$_GET['Heute_a'];
if(isset($_GET) && isset($_GET['Heute_e']))
	$Heute_e=$_GET['Heute_e'];

// Transaktion "Termine heute" ---> Wirklich nur heute
if($GLOBALS['fxpglobals']['tr'] == 49)
{
	$Heute_a=$today;
	$Heute_e=substr($Heute_a,0,8).'240000';
}
// Kontrollzentrum
else if(!isset($_GET['Heute_a']) && !isset($_POST['Button_Speichern']))
{
	$today_is=$today;
	$today_wd=formatdatetime('w', $today_is);
	$Heute_a=substr(time_add($today_is, (1 - $today_wd), 'D'), 0, 8).'000000';
	$Heute_e=substr(time_add($Heute_a, $GLOBALS['fxpglobals']['settings']['stdzeitspanne']-1, 'D'), 0, 8).'240000';
}

$maske185_head=meldung(325).' <font class="s2 grey">('.get_entry($Heute_a, 'datum');
if(substr($Heute_a,0,8) != substr($Heute_e,0,8))
	$maske185_head .= ' - '.get_entry($Heute_e,'datum');
$maske185_head .= ')</font>';

// Elter der Person
$has_parent=get_realelter();

// Wenn die Person Vorgesetzer von jemandem ist, wird die Abfrage auf Urlaubsanfragen seiner
// "Untergebenen", sowie deren Krankheitstage, Überstundenauszahlung, Gleitzeit etc. erweitert
$is_parent=false;
$all_personal=array();
// Alle untergeordneten Personen einlesen
$par_csv=$GLOBALS['fxpglobals']['person'];
// ...Elterdaten von Ansprechpartnern ermitteln: 620->276=Auftraggeber, 841->1372=Kontakt, 2092->2091=Lieferant
$ptyp=(int)$GLOBALS['fxpglobals']['persdat']['personentyp'];
if(($ptyp == FXP_PT_CUSTOMER_CP) || ($ptyp == FXP_PT_CONTACT_CP) || ($ptyp == FXP_PT_PARTNER_CP))
	$par_csv .= ",".(int)$GLOBALS['fxpglobals']['persdat']['elter'];
$sql="SELECT personen_id FROM personen WHERE elter IN (".$par_csv.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
$superior=db_values($sql,'*');
//fxDebug($superior, 'Untergeordnete: '.$sql);
if(fxIsArray($superior))
{
	$is_parent=true;
	foreach($superior as $p)
		$all_personal[$p['personen_id']]=true;
}
//fxDebug($all_personal, '$all_personal');

// Alle "Untergebenen" in ein Array
$upids='';
if(sizeof($all_personal))
{
	foreach($all_personal as $upid => $dmy)
	{
		if($upids)
			$upids .= ',';
		$upids .= $upid;
	}
}

// Prüfung, ob es offene Fertigstellungsmeldungen zu bereits abgeschlossenen Projekten gibt?
$sql="SELECT t.termin_id,t.projekt_id FROM termine t, projekte p WHERE t.terminart=1131 AND t.hat_bestaetigt IS NULL AND t.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projekt_id=t.projekt_id AND p.projekt_status=".FXP_PS_COMPLETED." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
if(fxIsArray($tmp))
{
	// ...Ja, dann Fertigstellungsmeldungen löschen
	$tcsv='';
	foreach($tmp as $t)
		$tcsv .= ','.$t['termin_id'];
	$sql="DELETE FROM termine WHERE termin_id IN (".substr($tcsv,1).") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	@db_x($sql);
}


if(fxIsArray($_POST) && ($_POST['Button_okay'] || $_POST['Button_Speichern']))
{
	// Start der Transaktion
	db_rollback('start');

	$settings_backup=$GLOBALS['fxpglobals']['settings'];
	$lang_backup=$GLOBALS['fxpglobals']['lang'];

	// Variablen leeren
	$GLOBALS['_ea']=array();
	$GLOBALS['_ua']=array();
	$GLOBALS['_ma']=array();

	$appnames=array();

	$ok='';
	$ko='';
	$done='';

	// Fehltagsanträge/Projektmitteilungen/Einladung genehmigen oder ablehnen
	if(!$err && fxIsArray($_POST['genehmigen'][$mc]))
	{
		// Get sender data
		$sender=$GLOBALS['fxpglobals']['person'];
		if(!isset($GLOBALS['_ua'][$sender]))
		{
			$pdata=get_email_adress($sender,true,true);
			if(fxIsArray($pdata) && fxIsArray($pdata[$sender]))
				$GLOBALS['_ua'][$sender]=$pdata[$sender];
			else
				$GLOBALS['_ua'][$sender]=false;
		}
		if(fxIsArray($GLOBALS['_ua'][$sender]))
			$sender_name=$GLOBALS['_ua'][$sender]['name2'];
		else
			$sender_name='('.$sender.')';
//fxDebug($GLOBALS['_ua'][$sender], '$sender='.$sender.' -- '.$sender_name);

		// Neuberechnung der Auslastung initialisieren
		$updpi=array();

		foreach($_POST['genehmigen'][$mc] as $ln => $ar_mode)
		{
			$appid=(int)$_POST['Termin_ID'][$mc][$ln];
			if(!$appid || (($ar_mode != 2291) && ($ar_mode != 2292)))
				continue;

			// Get appointment data
			$sql  = "SELECT termin_id, personen_id, terminart, projekt_id, name_termin, urlaub, startzeit, endzeit, stunden, person_angelegt, ansprechpartner,";
			$sql .= " bestaetigung_anmerk AS anmerkung FROM termine WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$appdata=db_values($sql);
//fxDebug($appdata,$sql);
			if(!fxIsArray($appdata))
				continue;

			$vacdiff=(float)$appdata['urlaub'];

			$invitation=false;
			if($appdata['terminart'] == FXP_AT_INVITATION)
				$invitation=true;
			else
			{
				$sql="SELECT termin_id FROM termine WHERE mastertermin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND terminart=".FXP_AT_INVITATION;
				$eex=db_value($sql);
				if($eex)
					$invitation=true;
			}

			// Get originator data
			$originator=(int)$appdata['person_angelegt'];
			if(!isset($GLOBALS['_ua'][$originator]))
			{
				$pdata=get_email_adress($originator,true,true);
				if(fxIsArray($pdata) && fxIsArray($pdata[$originator]))
					$GLOBALS['_ua'][$originator]=$pdata[$originator];
				else
					$GLOBALS['_ua'][$originator]=false;
			}
			if(fxIsArray($GLOBALS['_ua'][$originator]))
				$originator_name=$GLOBALS['_ua'][$originator]['name2'];
			else
				$originator_name='('.$originator.')';
//fxDebug($GLOBALS['_ua'][$originator], '$originator='.$originator.' -- '.$originator_name);

			// Get recipient data
			$recipient=(int)$_POST['Personen_ID'][$mc][$ln];
			if(!isset($GLOBALS['_ua'][$recipient]))
			{
				$pdata=get_email_adress($recipient,true,true);
				if(fxIsArray($pdata) && fxIsArray($pdata[$recipient]))
					$GLOBALS['_ua'][$recipient]=$pdata[$recipient];
				else
					$GLOBALS['_ua'][$recipient]=false;
			}
			if(fxIsArray($GLOBALS['_ua'][$recipient]))
				$recipient_name=$GLOBALS['_ua'][$recipient]['name2'];
			else
				$recipient_name='('.$recipient.')';
//fxDebug($GLOBALS['_ua'][$recipient], '$recipient='.$recipient.' -- '.$recipient_name);

			// Project info for "Project Messages"
			$prj_name=$appdata['projekt_id'];
			$pia=array();
			if($appdata['terminart'] == FXP_AT_PROJECTMESSAGE)
			{
				$sql  = "SELECT p.vorgangsnummer, p.name_projekt, p.projektart, p.beschreibung, z.soll_beginn, z.soll_ende, z.ist_beginn";
				$sql .= " FROM projekte p, zeitdaten z";
				$sql .= " WHERE p.projekt_id=".$appdata['projekt_id']." AND z.projekt_id=".$appdata['projekt_id']." AND z.zeitart=102";
				$sql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$pia=db_values($sql);
				if(fxIsArray($pia))
					$prj_name=no_null($pia['vorgangsnummer']).' - '.$pia['name_projekt'];
			}

			// Approved/Accepted
			if($ar_mode == 2291)
			{
				// ...Project Message: Task completion approved [TESTED/OK]
				if($appdata['terminart'] == FXP_AT_PROJECTMESSAGE)
				{
					// Complete task (+ parent projects + set completion date + activate dependent tasks)
					if(!$err)
					{
//echo('Execution of <b>set_status('.$appdata['projekt_id'].', 301);</b><br />');
						$err=set_status($appdata['projekt_id'],FXP_PS_COMPLETED);
						// 174: The project $var1 could not be "completed"!
						if($err)
							error_msg(174,false,'<b>'.$prj_name.'</b>');
					}

					// Update appointment
					if(!$err)
					{
						// 83: Accepted and confirmed by $var1 on $var2 at $var3 o'clock
						if(!isset($acc_note))
							$acc_note="'1-".convert_string(meldung(83,false,'<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit')), 'todb')."'";

						$sql  = "UPDATE termine SET hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."', bestaetigung_anmerk=".$acc_note.", erledigt=1";
						$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
//fxDebug($err,$sql);
					}

					// Create email
					if(!$err && check_mail(1728,'MA') && $originator && fxIsArray($pia))
					{
						$pers=$originator;
						if(fxIsArray($GLOBALS['_ua'][$pers]) && strlen($GLOBALS['_ua'][$pers]['email']))
						{
							$email=$GLOBALS['_ua'][$pers]['email'];
							if(!isset($GLOBALS['_ea'][$pers]))
								$GLOBALS['_ea'][$pers]=array('email'=>$email);

							$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$pers]['settings'];
							$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$pers]['settings']['sprache'];
							$lang=$GLOBALS['fxpglobals']['lang'];

							// the reported <b>completion</b> of task $var2 by you on $var1<br />has been checked by <b>$var3</b> and <b style="color:#$var5;">approved</b> on $var4!<br /><br />The task has been completed and all the following dependent tasks have been activated.
							$md='email_prj_completed';

							if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
								$GLOBALS['_ma'][$md.'_'.$lang]=get_message($md);

							if(!isset($GLOBALS['_ea'][$pers][$md.$appdata['projekt_id']]))
							{
								$title=meldung(2381,0,'['.$prj_name.']');	// 2381: Task $var1 has been completed
								$data=array(0=>strtr($GLOBALS['_ma'][$md.'_'.$lang], array('$var1'=>get_entry($appdata['startzeit'],'datetime'), '$var2'=>$prj_name, '$var3'=>$sender_name, '$var4'=>get_entry($GLOBALS['datetime'],'datetime'), '$var5'=>colorCode('green'))));

								$GLOBALS['_ea'][$pers][$md.$appdata['projekt_id']]=array
								(
									'title'	=> $title,
									'data'	=> $data
								);
							}

							$GLOBALS['fxpglobals']['settings']=$settings_backup;
							$GLOBALS['fxpglobals']['lang']=$lang_backup;
						}
					}

					// Create project mails
					if(!$err)
					{
						$eua=sendProjectMails($GLOBALS['_ea'],$GLOBALS['_ua'],false);
						$GLOBALS['_ea']=$eua['ea'];
						$GLOBALS['_ua']=$eua['ua'];
					}
				}

				// ...Invitation accepted [TESTED/OK]
				else if($invitation)
				{
					$at=substr($appdata['anmerkung'],0,1);
					if($at !== '1')
					{
						// Update appointment
						if(!$err)
						{
							// 194: Invitation accepted by $var1 on $var2 at $var3 o'clock
							if(!isset($ok_itext))
								$ok_itext=meldung(194, false, '<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit'));

							$sql  = "UPDATE termine SET hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."', bestaetigung_anmerk='1-".convert_string($ok_itext,'todb')."'";
							$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
							$err=db_x($sql);
//fxDebug($err, $sql);
						}

						// Send invitation acceptance e-mail to contact?
						if(!$err && check_mail(1734,'MA'))
						{
							$pers=$appdata['ansprechpartner'];
							$md='inv_acc';
							$marr=sendInvitationMails($pers, $appid, $md, false);
							if(!isset($GLOBALS['_ea'][$pers]))
								$GLOBALS['_ea'][$pers]=array('email'=>$marr['email']);
							if(!isset($GLOBALS['_ea'][$pers][$md.$appid]))
							{
								$GLOBALS['_ea'][$pers][$md.$appid]=array
								(
									'title'	=> $marr['title'],
									'salut'	=> "",
									'data'	=> array(0=>$marr['data'])
								);
							}
						}
					}
				}

				// Appointment applications accepted [TESTED/OK]
				else
				{
					// Change applications to absence days
					$new_app=getAcceptedApplicationID($appdata['terminart']);
					if(!$new_app)
						continue;

					if(!$appnames[$new_app])
						$appnames[$new_app]=meldung($new_app, false);

					// 83: Accepted and confirmed by $var1 on $var2 at $var3 o'clock
					if(!isset($acc_note))
						$acc_note="'1-".convert_string(meldung(83,false,'<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit')), 'todb')."'";

					$sql  = "UPDATE termine SET terminart=".$new_app.", hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."',";
					$sql .= " bestaetigung_anmerk=".$acc_note.", name_termin='".convert_string($appnames[$new_app],'todb')."', erledigt=1";
					$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
//fxDebug($err, $sql);

					$overtime=false;
					if($new_app == FXP_AT_OVERTIMEOUTPAY)
						$overtime=true;

					// Recalculate vacation days
					if(!$err && ($new_app == FXP_AT_VACATION) && ($vacdiff != 0.00))
					{
//echo('Execution of <b>reset_resturlaub</b>('.$recipient.', '.$appdata['startzeit'].')<br />');
						$err=reset_resturlaub($recipient, $appdata['startzeit']);
						// 479: Error while calculating the remaining days of vacation for $var1!
						if($err)
							error_msg(479, false, '<b>'.$recipient_name.'</b>');
					}

					// Create flexday
					if(!$err && ($new_app == FXP_AT_SLIDINGDAY))
					{
						$data_array=array(
							'personen_id'	=> $recipient,
							'ist_beginn'	=> $appdata['startzeit'],
							'ist_ende'		=> $appdata['endzeit'],
							'pause'			=> (int)((int)time_diff($appdata['startzeit'], $appdata['endzeit'], 'S') - (int)($appdata['stunden']*3600)),
							'zeitaufwand'	=> (int)($appdata['stunden']*3600)
						);

//fxDebug($data_array, 'Execution of <b>gleittag</b>('.$appid.', $data_array)');
						$err=gleittag($appid, $data_array);
						if($err)
							error_msg($err, false);
					}

					// Delete balance or capping datasets
					if(!$err && (($new_app == FXP_AT_SLIDINGDAY) || ($new_app == FXP_AT_OVERTIMEOUTPAY)))
					{
//echo('Execution of <b>bilanz_zeitkonto_loeschen</b>('.$recipient.', '.$appdata['startzeit'].')<br />');
						$err=bilanz_zeitkonto_loeschen($recipient, $appdata['startzeit']);
						// 1868: Error while deleting old balance or capping datasets!
						if($err)
							error_msg(1868,false);
					}

					$md='email_app_ar';

					// Send acceptance mail to applicant (=recipient)
					if(!$err && fxIsArray($GLOBALS['_ua'][$recipient]) && strlen($GLOBALS['_ua'][$recipient]['email']) && check_mail(1733,'MA'))
					{
						$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$recipient]['settings'];
						$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$recipient]['settings']['sprache'];
						$lang=$GLOBALS['fxpglobals']['lang'];

						if(!isset($GLOBALS['_ea'][$recipient]))
							$GLOBALS['_ea'][$recipient]=array('email'=>$GLOBALS['_ua'][$recipient]['email']);
						if(!isset($GLOBALS['_ea'][$recipient][$md.$appid]))
						{
							// Genehmigung^Ablehnung^{day} Tag am {timespan}^{days} Tage im Zeitraum {timespan}^{hours} Stunden für^von^&uuml;ber {timespan} wurde von {sender} {status}^genehmigt^abgewiesen
							// Approval^Rejection^{day} day on {timespan}^{days} days in the time span {timespan}^{hours} hours for^from^for {timespan} has been {status} by {sender}^approved^rejected
							if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
								$GLOBALS['_ma'][$md.'_'.$lang]=explode('^',get_message($md));
//fxDebug($GLOBALS['_ma'][$md.'_'.$lang], $md);

							$appname=meldung($appdata['terminart']);
							$month=meldung(17+(int)substr($appdata['startzeit'],4,2), false).' '.substr($appdata['startzeit'],0,4);
							$timespan=get_entry($appdata['startzeit'],'datetime');
							if(substr($appdata['endzeit'],0,8) != substr($appdata['startzeit'],0,8))
								$timespan .= ' - '.get_entry($appdata['endzeit'],'datetime');
							else
								$timespan .= '-'.get_entry($appdata['endzeit'],'zeit');

							$title=$GLOBALS['_ma'][$md.'_'.$lang][0].': '.$appname.' - '.$recipient_name.': ';	// Approval: Vacation application - Allison Mack:
							if($overtime)
								$title .= strtr($GLOBALS['_ma'][$md.'_'.$lang][4], array('{hours}'=>get_entry($appdata['stunden'],'dezimal'))).' '.$month;
							else if($appdata['urlaub'] <= 1.00)
								$title .= strtr($GLOBALS['_ma'][$md.'_'.$lang][2], array('{day}'=>get_entry($appdata['urlaub'],'dezimal'), '{timespan}'=>$timespan));
							else
								$title .= strtr($GLOBALS['_ma'][$md.'_'.$lang][3], array('{days}'=>get_entry($appdata['urlaub'],'dezimal'), '{timespan}'=>$timespan));

							if($overtime)
								$ts=strtr($GLOBALS['_ma'][$md.'_'.$lang][4], array('{hours}'=>'<i>'.get_entry($appdata['stunden'],'dezimal').'</i>')).' <i>'.$month.'</i>';
							else if($appdata['urlaub'] <= 1.00)
								$ts=strtr($GLOBALS['_ma'][$md.'_'.$lang][2], array('{day}'=>'<i>'.get_entry($appdata['urlaub'],'dezimal').'</i>', '{timespan}'=>'<i>'.$timespan.'</i>'));
							else
								$ts=strtr($GLOBALS['_ma'][$md.'_'.$lang][3], array('{days}'=>'<i>'.get_entry($appdata['urlaub'],'dezimal').'</i>', '{timespan}'=>'<i>'.$timespan.'</i>'));

							$status  = '<br /><br /><table cellpadding="16" cellspacing="1" bgcolor="#666666" style="border-radius:16px;box-shadow:8px 8px 8px #666666;">';
							$status .= '<tr><td bgcolor="#'.colorCode('green').'" align="center" style="border-radius:16px;">';
							$status .= '<b style="font-size:32pt;color:#ffffff;">&nbsp;'.$GLOBALS['_ma'][$md.'_'.$lang][7].'&nbsp;</b>';
							$status .= '</td></tr></table>';
							if($lang == 7)
								$status .= '<br /><br />';

							$data  = '<b>'.$appname.'</b> ';
							$data .= strtr($GLOBALS['_ma'][$md.'_'.$lang][6], array('{timespan}'=>$ts, '{sender}'=>'<b style="color:#'.colorCode('blue').';">'.$sender_name.'</b>', '{status}'=>$status));

							$GLOBALS['_ea'][$recipient][$md.$appid]=array
							(
								'title'	=> $title,
								'ics'	=> $appid,
								'data'	=> array(0=>$data)
							);
						}

						$GLOBALS['fxpglobals']['settings']=$settings_backup;
						$GLOBALS['fxpglobals']['lang']=$lang_backup;
					}

					if(!$overtime)
					{
						// Get tasks with effort>0 in which the applicant would be absent
						$mtasks=getAbsentTasks($recipient,$appdata['startzeit'],$appdata['endzeit']);
						$prpa=getMailPersons(array(), 1733, $recipient, false, $appdata['startzeit'], $appdata['endzeit']);

						// E-Mail to project leaders etc.
						if(sizeof($mtasks) && fxIsArray($prpa))
						{
							// Variables for e-mail function
							$GLOBALS['applicant_name']=$recipient_name;
							$GLOBALS['ftart']=$new_app;
							$GLOBALS['request']=false;
							$GLOBALS['old']=$appdata;
							$GLOBALS['days']=$appdata['urlaub'];
							$GLOBALS['hours']=$appdata['stunden'];
							$GLOBALS['db_start']=$appdata['startzeit'];
							$GLOBALS['db_end']=$appdata['endzeit'];
							$GLOBALS['adesc']=$appdata['beschreibung'];

							foreach($prpa as $dpers => $dtype)
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
									if(!isset($GLOBALS['_ea'][$dpers]))
										$GLOBALS['_ea'][$dpers]=array('email'=>$GLOBALS['_ua'][$dpers]['email']);
									if(!isset($GLOBALS['_ea'][$dpers][$md.$appid]))	// only if not already sent
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
											$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$dpers]['settings'];
											$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
											$lang=$GLOBALS['fxpglobals']['lang'];

											// Create e-mail
											absentAppMail($dpers, true, FXP_CREATE);

											$GLOBALS['_ea'][$dpers][$md.$appid]=array(
												'title'	=> $title,
												'ics'	=> $appid,
												'data'	=> array(0=>$content)
											);

											$GLOBALS['fxpglobals']['settings']=$settings_backup;
											$GLOBALS['fxpglobals']['lang']=$lang_backup;
										}
									}
								}
							}
						}
					}

					// Recalculate project utilization for this person
					if(!$err)
					{
						$updpersid=(int)$recipient;
						if(!fxIsArray($updpi[$updpersid]))
							$updpi[$updpersid]=$appdata['startzeit'];
						else
							$updpi[$updpersid]=min($appdata['startzeit'], $updpi[$updpersid]);
					}
				}

				if($err)
				{
					// 87: $var1 could not be "<b>approved/accepted</b>"!
					error_msg(87, false, $_POST['Pname'][$mc][$ln]);
					break;
				}

				$ok .= '<li>'.$_POST['Pname'][$mc][$ln].'</li>';
			}

			// Abgelehnt
			if($ar_mode == 2292)
			{
				// Project Message: Task completion [TESTED/OK]
				if($appdata['terminart'] == FXP_AT_PROJECTMESSAGE)
				{
					// Update appointment
					if(!$err)
					{
						// 84: Denied/Rejected by $var1 on $var2 at $var3 o'clock
						if(!isset($rej_note))
							$rej_note="'0-".convert_string(meldung(84,false,'<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit')), 'todb')."'";

						$sql  = "UPDATE termine SET hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."', bestaetigung_anmerk=".$rej_note.", erledigt=1";
						$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
//fxDebug($err, $sql);
					}

					// Create e-mail
					if(!$err && check_mail(1728,'MA') && $originator && fxIsArray($pia))
					{
						$pers=$originator;

						if(fxIsArray($GLOBALS['_ua'][$pers]) && strlen($GLOBALS['_ua'][$pers]['email']))
						{
							$email=$GLOBALS['_ua'][$pers]['email'];
							if(!isset($GLOBALS['_ea'][$pers]))
								$GLOBALS['_ea'][$pers]=array('email'=>$email);

							$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$pers]['settings'];
							$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$pers]['settings']['sprache'];
							$lang=$GLOBALS['fxpglobals']['lang'];

							// the reported <b>completion</b> of task $var2 by you on $var1<br />has been checked by <b>$var3</b> and <b style="color:#$var5;">rejected</b> on $var4!<br /><br />Please contact $var3 about possible rectifications or remaining works.
							$md='email_prj_rework';

							if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
								$GLOBALS['_ma'][$md.'_'.$lang]=get_message($md);

							if(!isset($GLOBALS['_ea'][$pers][$md.$appdata['projekt_id']]))
							{
								$title=meldung(2577,0,'['.$prj_name.']');	// 2577: Completion of task $var1 has been rejected
								$data=array(0=>strtr($GLOBALS['_ma'][$md.'_'.$lang], array('$var1'=>get_entry($appdata['startzeit'],'datetime'), '$var2'=>$prj_name, '$var3'=>$sender_name, '$var4'=>get_entry($GLOBALS['datetime'],'datetime'), '$var5'=>colorCode('red'))));

								$GLOBALS['_ea'][$pers][$md.$appdata['projekt_id']]=array
								(
									'title'	=> $title,
									'data'	=> $data
								);
							}

							$GLOBALS['fxpglobals']['settings']=$settings_backup;
							$GLOBALS['fxpglobals']['lang']=$lang_backup;
						}
					}
				}

				// Invitation refusal [TESTED/OK]
				else if($invitation)
				{
					$at=substr($appdata['anmerkung'],0,1);
					if($at !== '0')
					{
						// Update appointment
						if(!$err)
						{
							// 196: Invitation refused by $var1 on $var2 at $var3 o'clock
							if(!isset($ref_note))
								$ref_note="'0-".convert_string(meldung(196, false, '<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit')), 'todb')."'";

							$sql  = "UPDATE termine SET hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."', bestaetigung_anmerk=".$ref_note.", erledigt=1";
							$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
							$err=db_x($sql);
//fxDebug($err, $sql);
						}

						// Send invitation refusal e-mail to contact?
						if(!$err && check_mail(1734,'MA'))
						{
							$pers=$appdata['ansprechpartner'];
							$md='inv_refuse';
							$marr=sendInvitationMails($pers, $appid, $md, false);
							if(!isset($GLOBALS['_ea'][$pers]))
								$GLOBALS['_ea'][$pers]=array('email'=>$marr['email']);
							if(!isset($GLOBALS['_ea'][$pers][$md.$appid]))
							{
								$GLOBALS['_ea'][$pers][$md.$appid]=array
								(
									'title'	=> $marr['title'],
									'salut'	=> "",
									'data'	=> array(0=>$marr['data'])
								);
							}
						}
					}
				}

				// Appointment applications rejected [TESTED/OK]
				else
				{
					// 86: Denial/Rejection
					if(!strlen($rej_subject))
						$rej_subject=meldung(86,false);

					// 84: Denied/Rejected by $var1 on $var2 at $var3 o'clock
					if(!isset($rej_note))
						$rej_note="'0-".convert_string(meldung(84,false,'<b>'.$sender_name.'</b>',get_entry($GLOBALS['datetime'],'datum'),get_entry($GLOBALS['datetime'],'zeit')), 'todb')."'";

					// Change applications to messages
						$sql  = "UPDATE termine SET terminart=".FXP_AT_MESSAGE.", startzeit='".$appdata['startzeit']."', endzeit='".$appdata['startzeit']."',";
						$sql .= " hat_bestaetigt=".$GLOBALS['fxpglobals']['person'].", bestaetigt_am='".$GLOBALS['datetime']."', bestaetigung_anmerk=".$rej_note.",";
						$sql .= " name_termin='".convert_string($rej_subject.': '.$appdata['name_termin'],'todb')."', urlaub=0, stunden=NULL, erledigt=1";
						$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
					$err=db_x($sql);
//fxDebug($err, $sql);

					$overtime=false;
					if($appdata['terminart'] == FXP_AT_OVERTIMEOUTPAYREQUEST)
						$overtime=true;

					$md='email_app_ar';

					// Send rejection mail to applicant (=recipient)
					if(!$err && fxIsArray($GLOBALS['_ua'][$recipient]) && strlen($GLOBALS['_ua'][$recipient]['email']) && check_mail(1733,'MA'))
					{
						$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$recipient]['settings'];
						$GLOBALS['fxpglobals']['lang']=$GLOBALS['_ua'][$recipient]['settings']['sprache'];
						$lang=$GLOBALS['fxpglobals']['lang'];

						if(!isset($GLOBALS['_ea'][$recipient]))
							$GLOBALS['_ea'][$recipient]=array('email'=>$GLOBALS['_ua'][$recipient]['email']);
						if(!isset($GLOBALS['_ea'][$recipient][$md.$appid]))
						{
							// Genehmigung^Ablehnung^{day} Tag am {timespan}^{days} Tage im Zeitraum {timespan}^{hours} Stunden für^von^&uuml;ber {timespan} wurde von {sender} {status}^genehmigt^abgewiesen
							// Approval^Rejection^{day} day on {timespan}^{days} days in the time span {timespan}^{hours} hours for^from^for {timespan} has been {status} by {sender}^approved^rejected
							if(!isset($GLOBALS['_ma'][$md.'_'.$lang]))
								$GLOBALS['_ma'][$md.'_'.$lang]=explode('^',get_message($md));
//fxDebug($GLOBALS['_ma'][$md.'_'.$lang], $md);

							$appname=meldung($appdata['terminart']);
							$month=meldung(17+(int)substr($appdata['startzeit'],4,2), false).' '.substr($appdata['startzeit'],0,4);
							$timespan=get_entry($appdata['startzeit'],'datetime');
							if(substr($appdata['endzeit'],0,8) != substr($appdata['startzeit'],0,8))
								$timespan .= ' - '.get_entry($appdata['endzeit'],'datetime');
							else
								$timespan .= '-'.get_entry($appdata['endzeit'],'zeit');

							$title=$GLOBALS['_ma'][$md.'_'.$lang][1].': '.$appname.' - '.$recipient_name.': ';	// Rejection: Vacation application - Allison Mack:
							if($overtime)
								$title .= strtr($GLOBALS['_ma'][$md.'_'.$lang][4], array('{hours}'=>get_entry($appdata['stunden'],'dezimal'))).' '.$month;
							else if($appdata['urlaub'] <= 1.00)
								$title .= strtr($GLOBALS['_ma'][$md.'_'.$lang][2], array('{day}'=>get_entry($appdata['urlaub'],'dezimal'), '{timespan}'=>$timespan));
							else
								$title .= strtr($GLOBALS['_ma'][$md.'_'.$lang][3], array('{days}'=>get_entry($appdata['urlaub'],'dezimal'), '{timespan}'=>$timespan));

							if($overtime)
								$ts=strtr($GLOBALS['_ma'][$md.'_'.$lang][4], array('{hours}'=>'<i>'.get_entry($appdata['stunden'],'dezimal').'</i>')).' <i>'.$month.'</i>';
							else if($appdata['urlaub'] <= 1.00)
								$ts=strtr($GLOBALS['_ma'][$md.'_'.$lang][2], array('{day}'=>'<i>'.get_entry($appdata['urlaub'],'dezimal').'</i>', '{timespan}'=>'<i>'.$timespan.'</i>'));
							else
								$ts=strtr($GLOBALS['_ma'][$md.'_'.$lang][3], array('{days}'=>'<i>'.get_entry($appdata['urlaub'],'dezimal').'</i>', '{timespan}'=>'<i>'.$timespan.'</i>'));

							$status  = '<br /><br /><table cellpadding="16" cellspacing="1" bgcolor="#666666" style="border-radius:16px;box-shadow:8px 8px 8px #666666;">';
							$status .= '<tr><td bgcolor="#'.colorCode('red').'" align="center" style="border-radius:16px;">';
							$status .= '<b style="font-size:32pt;color:#ffffff;">&nbsp;'.$GLOBALS['_ma'][$md.'_'.$lang][8].'&nbsp;</b>';
							$status .= '</td></tr></table>';
							if($lang == 7)
								$status .= '<br /><br />';

							$data  = '<b>'.$appname.'</b> ';
							$data .= strtr($GLOBALS['_ma'][$md.'_'.$lang][6], array('{timespan}'=>$ts, '{sender}'=>'<b style="color:#'.colorCode('blue').';">'.$sender_name.'</b>', '{status}'=>$status));

							$GLOBALS['_ea'][$recipient][$md.$appid]=array
							(
								'title'	=> $title,
								'data'	=> array(0=>$data)
							);
						}

						$GLOBALS['fxpglobals']['settings']=$settings_backup;
						$GLOBALS['fxpglobals']['lang']=$lang_backup;
					}

					if(!$overtime)
					{
						// Get tasks with effort>0 in which the applicant would be available again
						$mtasks=getAbsentTasks($recipient,$appdata['startzeit'],$appdata['endzeit']);
						$prpa=getMailPersons(array(), 1733, $recipient, false, $appdata['startzeit'], $appdata['endzeit']);

						// E-Mail to project leaders etc.
						if(sizeof($mtasks) && fxIsArray($prpa))
						{
							// Variables for e-mail function
							$GLOBALS['applicant_name']=$recipient_name;
							$GLOBALS['ftart']=$appdata['terminart'];
							$GLOBALS['request']=true;
							$GLOBALS['old']=$appdata;
							$GLOBALS['days']=$appdata['urlaub'];
							$GLOBALS['hours']=$appdata['stunden'];
							$GLOBALS['db_start']=$appdata['startzeit'];
							$GLOBALS['db_end']=$appdata['endzeit'];
							$GLOBALS['adesc']=$appdata['beschreibung'];

							foreach($prpa as $dpers => $dtype)
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
									if(!isset($GLOBALS['_ea'][$dpers]))
										$GLOBALS['_ea'][$dpers]=array('email'=>$GLOBALS['_ua'][$dpers]['email']);
									if(!isset($GLOBALS['_ea'][$dpers][$md.$appid]))	// only if not already sent
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
											$GLOBALS['fxpglobals']['settings']=$GLOBALS['_ua'][$dpers]['settings'];
											$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];
											$lang=$GLOBALS['fxpglobals']['lang'];

											// Create e-mail
											absentAppMail($dpers, true, FXP_DELETE);

											$GLOBALS['_ea'][$dpers][$md.$appid]=array(
												'title'	=> $title,
												'data'	=> array(0=>$content)
											);

											$GLOBALS['fxpglobals']['settings']=$settings_backup;
											$GLOBALS['fxpglobals']['lang']=$lang_backup;
										}
									}
								}
							}
						}
					}
				}

				if($err)
				{
					// 88: $var1 could not be "<b>denied/rejected</b>"!
					error_msg(88, false, $_POST['Pname'][$mc][$ln]);
					break;
				}

				$ko .= '<li>'.$_POST['Pname'][$mc][$ln].'</li>';
			}
		}

		// Neuberechnung der Auslastung
		if(!$err && fxIsArray($updpi) && sizeof($updpi))
		{
			foreach($updpi as $upi => $sdatum)
			{
				if($err)
					continue;

				$err=update_maplanung('person', $GLOBALS['fxpglobals']['client'], 0, $upi, $sdatum);
			}
		}
	}

	// Termine erledigt
	if(!$err && fxIsArray($_POST['erledigt'][$mc]))
	{
		foreach($_POST['erledigt'][$mc] as $ln => $v)
		{
			$appid=(int)$_POST['Termin_ID'][$mc][$ln];
			if(!$appid || !$v)
				continue;

			$sql  = "UPDATE termine SET erledigt=1, verschieben=0";
			$sql .= " WHERE termin_id=".$appid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($sql);
			if($err)
			{
				error_msg(79, false, $_POST['Pname'][$mc][$ln]);	// 79: $var1 konnte nicht als "erledigt" gekennzeichnet werden!
				break;
			}
			$done .= '<li>'.$_POST['Pname'][$mc][$ln].'</li>';
		}
	}

	// Bei Fehler Rollback durchführen
	$merr=false;
	if($err)
		db_rollback('error');
	else
	{
		db_rollback('ok');
		if(strlen($done) || strlen($ko) || strlen($ok))
		{
			// Erfolgsmeldungen
			if(strlen($done))
				error_msg(80, false, $done);	// 80: Nachfolgende Termine wurden als "erledigt" gekennzeichnet:<UL>$var1</UL>
			if(strlen($ko))
				error_msg(81, false, $ko);	// 81: Nachfolgende Anträge/Meldungen wurden abgelehnt:<UL>$var1</UL>
			if(strlen($ok))
				error_msg(82, false, $ok);	// 82: Nachfolgende Anträge/Meldungen wurden genehmigt:<UL>$var1</UL>

			$sent=0;
			// Create and send emails
			if(sizeof($GLOBALS['_ea']))
			{
//fxDebug($GLOBALS['_ea'],'$GLOBALS[\'_ea\']');
				$sc=sendPersonMails($GLOBALS['_ea'],$GLOBALS['_ua']);
				if($sc === false)
					$merr=true;
				else
					$sent += $sc;
			}
		}
		else
			error_msg(1642, false);	// 1642: Es wurden keine Einträge angehakt.
	}

	$GLOBALS['fxpglobals']['settings']=$settings_backup;
	$GLOBALS['fxpglobals']['lang']=$lang_backup;
}

$l=0;

// Erstellen der kompletten SQL-Abfrage
$lsql  = "SELECT t.termin_id, t.mastertermin_id, t.name_termin, t.beschreibung, t.terminart, t.ansprechpartner, t.person_angelegt,";
$lsql .= " t.startzeit, t.startzeit AS von, t.endzeit, t.endzeit AS bis, t.stunden, w.tabwert, w.id_sprache, t.erledigt,";
$lsql .= " t.projekt_id, t.zeit_id, p.pname, p.vorname, p.geschlecht, p.personen_id, t.wiedervorlagedb AS wiedervorlage,";
$lsql .= " t.verschieben, t.urlaub, t.treffpunkt, t.bestaetigung_anmerk AS anmerkung, t.wiederholung".$nl;
$lsql .= " FROM (personen p LEFT JOIN termine t ON t.personen_id=p.personen_id AND t.mandanten_id=p.mandanten_id) LEFT JOIN wertetab w ON w.wertetabid=t.terminart".$nl;
$lsql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND (t.archiv=0 OR t.archiv IS NULL) AND w.id_sprache=".$GLOBALS['fxpglobals']['lang'].$nl;
$lsql .= " AND w.mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")".$nl." AND ";
$lsql .= "(";

// Alle eigenen Termine...
$lsql .= "(t.personen_id=".$GLOBALS['fxpglobals']['person']." AND ";
$lsql .= "(";
// ...die im ausgewählten Zeitraum liegen...
$lsql .= "(t.startzeit<='".$Heute_e."' AND t.endzeit>='".$Heute_a."')";
// ...oder die im ausgewählten Zeitraum wiedervorzulegen sind...
$lsql .= " OR (t.wiedervorlagedb>='".$Heute_a."' AND t.wiedervorlagedb<='".$Heute_e."')";
// ...oder die verschiebbar sind
$lsql .= " OR t.verschieben=1";
// ...oder von unerledigten Projektmitteilungen
$lsql .= " OR (t.terminart=".FXP_AT_PROJECTMESSAGE." AND (t.erledigt IS NULL OR t.erledigt=0))";
// ...oder von zugesagten Terminaufgabeneinladungen, zu denen noch keine Zeiterfassung durchgeführt wurde
$lsql .= " OR (t.zeit_id=-1 AND t.projekt_id>0 AND (t.erledigt IS NOT NULL OR t.erledigt<>0))";
// ...oder von sämtlichen unerledigten Anträgen
$lsql .= " OR (t.terminart IN (".FXP_AT_VACATIONREQUEST.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SICKCALL.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_OVERTIMEOUTPAYREQUEST.") AND (t.erledigt=0 OR t.erledigt IS NULL))";
$lsql .= "))";

// Alle fremden Abwesenheitstermine...
$usql="";
// ...GF+VW sieht alle Personen
if($GLOBALS['fxpglobals']['persdat']['roles'][2] || $GLOBALS['fxpglobals']['persdat']['roles'][-1])
	$usql="(t.personen_id<>".$GLOBALS['fxpglobals']['person']." AND t.personen_id>0)";
// ...Rest nur die der direkt Untergeordneten
else if(strlen($upids))
	$usql="t.personen_id IN (".$upids.")";
if(strlen($usql))
{
	$flt=filter_person('from',true,false,'app_pers');
	$maske185_flt=$mskfilter;

	$usql .= " AND t.personen_id IN (SELECT p.personen_id".$flt.")";
	$lsql .= $nl." OR (".$usql." AND ";
	$lsql .= "(";
	// ...von Terminen, die im ausgewählten Zeitraum liegen...
	$lsql .= "((t.startzeit<='".$Heute_e."' AND t.endzeit>='".$Heute_a."') AND (";
	// ...entweder Abwesenheitstermine
	$lsql .= "t.terminart IN (".FXP_AT_ABSENT.",".FXP_AT_VACATION.",".FXP_AT_SPECIALVACATION.",".FXP_AT_SICK.",".FXP_AT_TRAINING.",".FXP_AT_SLIDINGDAY.",".FXP_AT_OVERTIMEOUTPAY.")";
	// ...oder von abgelehnten Abwesenheitsanträgen...
	$lsql .= " OR (t.terminart=".FXP_AT_MESSAGE." AND t.hat_bestaetigt IS NOT NULL)";
	// ...oder von einem selbst zuletzt geänderten Terminen...
	$lsql .= " OR t.aenderungs_id=".$GLOBALS['fxpglobals']['person'];
	$lsql .= "))";
	// ...oder von allen unerledigten Anträgen
	$lsql .= " OR (t.terminart IN (".FXP_AT_VACATIONREQUEST.",".FXP_AT_SPECIALVACATIONREQUEST.",".FXP_AT_SICKCALL.",".FXP_AT_TRAININGREQUEST.",".FXP_AT_SLIDINGDAYREQUEST.",".FXP_AT_OVERTIMEOUTPAYREQUEST.") AND (t.erledigt=0 OR t.erledigt IS NULL))";
	$lsql .= "))";
}

$lsql .= ")".$nl;

if($sort_person)
	$lsql .= " ORDER BY p.pname, p.vorname, t.startzeit DESC";
else
	$lsql .= " ORDER BY t.startzeit DESC, p.pname, p.vorname";

// Füllen der Listenwerte
$lstw=db_values($lsql, '*');
fxDebug($lstw,$lsql, 2);

// Literal-Icons Genehmigen/Ablehnen/Keine Aktion
$msort=meldung(1485);	// Nach [$var1] sortieren
$gal74=meldung(74);	// Genehmigen
$gal75=meldung(75);	// Ablehnen
$gal76=meldung(76);	// Keine Aktion
$genehmigen_lit='<img id="imgappr" src="'.$GLOBALS['gfxpath'].'tl_green.png" tooltip="'.$gal74.'" style="'.$glstyle.'cursor:help;"><img id="imgdecl" src="'.$GLOBALS['gfxpath'].'tl_red.png" tooltip="'.$gal75.'" style="'.$gmstyle.'cursor:help;"><img id="imgnot" src="'.$GLOBALS['gfxpath'].'tl_grey.png" tooltip="'.$gal76.'" style="'.$grstyle.'cursor:help;">';

// Einstellungswert, ob Verwaltung auch Abwesenheitsanträge genehmigen darf von Personen, welche einen anderen Vorgesetzten haben
$ap_can_appr_all=0;
if(strtolower(substr($GLOBALS['fxpglobals']['settings']['vw_alle_gen'], 0, 1)) == 'j')
	$ap_can_appr_all=1;

// Aufbereiten der Tabelle
if(fxIsArray($lstw))
{
	// All field columns
	$fa=array('Zeitraum', 'Person_alias', 'TabWert', 'Tage', 'genehmigen', 'erledigt', 'icon|0');

	// No right border in the following column
	fxf_maskColumnBorder(array('icon'), 0, -1, $mc);

	$GLOBALS['fxptdata']['lvalues'][$mc]=array();

	// Anzahl der anzuzeigenden Personen ermitteln
	$ppa=array();
	foreach($lstw as $lcnt => $lw)
	{
		$lstw[$lcnt]['name_termin']=$lw['name_termin'];
		$lstw[$lcnt]['beschreibung']=$lw['beschreibung'];
		$lstw[$lcnt]['anmerkung']=$lw['anmerkung'];

		$id=(int)$lw['personen_id'];
		if($lw['terminart'] == FXP_AT_PROJECTMESSAGE)
			$id=$lw['person_angelegt'];

		if(!fxIsArray($ppa[$id]))
		{
			$no_person++;

			$pdata=get_persondata($id,$GLOBALS['fxpglobals']['client'],false);
//fxDebug($pdata,'$id='.$id, 0);

			$var3=$pdata['name'];
			$name=$pdata['name2'];
			$lwname='&nbsp;'.person_pic($id).'&nbsp;';
			$ppa[$id]=array('var3'=>$var3, 'name'=>$name, 'lwname'=>$lwname);
		}
	}
//fxDebug($ppa, '$ppa', 0);

	if($no_person > 1)
	{
		if($sort_person)
		{
			$lit=get_text(1325, '20');	// Zeitraum
			$Zeitraum_lit='<img id="ccimg" src="'.$GLOBALS['gfxpath'].'sortd.png" align="right" tooltip="'.str_replace('$var1', $lit, $msort).'" style="margin-top:3px;cursor:pointer;" '.fxf_jsFunction('reloadApp', 'ccapp', 'appointments.inc', '__itr=49&sp=0&Heute_a='.$Heute_a.'&Heute_e='.$Heute_e.'&gj='.substr($Heute_a,0,4)).'>'.$lit;
		}
		else
		{
			$lit=get_text(21, '20');	// Person
			$Person_alias_lit='<img id="ccimg" src="'.$GLOBALS['gfxpath'].'sortd.png" align="right" tooltip="'.str_replace('$var1', $lit, $msort).'" style="margin-top:3px;cursor:pointer;" '.fxf_jsFunction('reloadApp', 'ccapp', 'appointments.inc', '__itr=49&sp=1&Heute_a='.$Heute_a.'&Heute_e='.$Heute_e.'&gj='.substr($Heute_a,0,4)).'>'.$lit;
		}
	}

	// Array mit Fehltagsanträge/Fehltagen-Terminarten
	$name_bck='';
	$ldisp=false;
	$prjdata_arr=array();
	$has_supervisor=array();

	fxDeleteTMP();
	$GLOBALS['mail_attachments']=array();
	$icpa=array();
	$icta=explode('|',meldung(3144));	// Termin im iCalendar-Format downloaden|Alle Termine im iCalendar-Format downloaden

	$lnc='';
	$pad=6;
	$fsz=5;
	$fsz=5;
	if($GLOBALS['fxpglobals']['tr'] == 14)	// Kontrollzentrum
	{
		$pad=4;
		$fsz=3;
	}

	// Liste zweimal durchlaufen: Zuerst mit aktueller Person, dann mit Rest
	for($ld=0; $ld<2; $ld++)
	{
		foreach($lstw as $lw)
		{
			$id=(int)$lw['personen_id'];
			if($lw['terminart'] == FXP_AT_PROJECTMESSAGE)
				$id=$lw['person_angelegt'];

			// Termine dieser Person anzeigen?
			if((!$ld && ($id != $GLOBALS['fxpglobals']['person'])) || ($ld && ($id == $GLOBALS['fxpglobals']['person'])))
				continue;

			// Keine Einladungstermine anzeigen, ausser eigene
			if(($lw['terminart'] == FXP_AT_INVITATION) && ($id != $GLOBALS['fxpglobals']['person']))
				continue;

			// Projektinfo
			$prjdata=false;
			if($lw['projekt_id'] > 0)
			{
				if(!isset($prjdata_arr[$lw['projekt_id']]))
					$prjdata_arr[$lw['projekt_id']]=db_values("SELECT vorgangsnummer, name_projekt, projektart, projekt_status, aufwand_ist FROM projekte WHERE projekt_id=".$lw['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				$prjdata=$prjdata_arr[$lw['projekt_id']];
				if(!fxIsArray($prjdata))
					continue;
				else
					$prjdata['name_projekt']=$prjdata['name_projekt'];
			}

			$lw['tage']=$lw['urlaub'];

			// Terminart
			$ist_fehltagsantrag=isAbsentApprovalAppointment($lw['terminart']);
			$ist_fehltag=isAbsentAppointment($lw['terminart']);

			$invitation=false;
			if($lw['terminart'] == FXP_AT_INVITATION)
				$invitation=true;
			else
			{
				$sql="SELECT termin_id FROM termine WHERE mastertermin_id=".$lw['termin_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND terminart=".FXP_AT_INVITATION;
				$eex=db_value($sql);
				if($eex)
					$invitation=true;
			}

			// Name
			$var3=$ppa[$id]['var3'];
			$col='223';
			$bcol='';
			if(!$ld || ($no_person == 1) || $sort_person)
			{
				if($var3 != $name_bck)
				{
					$name_bck=$var3;

					// Leerzeile
					if($l)
					{
						fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
						$l++;
					}

					$icpa[substr('0000'.$GLOBALS['fxpglobals']['client'],-4).'-'.substr('000000000'.$lw['personen_id'],-9).'-']=$l;

					// Farbe
					if(!$ld)
					{
						$col='fff';
						$bcol='linear-gradient(to bottom,#fff,#79aec7,#79aec7,#006b9f)';
					}
					$txt='<div class="s'.$fsz.'b tshadowdark" style="float:left;color:#'.$col.';">'.str_replace('>&nbsp;', ' style=border-top-left-radius:4px;>&nbsp;', $ppa[$id]['lwname']).'</div>';
					fxf_listSectHeader($mc, $l, $txt, $fcoll, $lcoll, $colspan, -2, 1, $bcol, 'padding:'.$pad.'px;');
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon']='~|';
					$l++;
				}
				$palias='';
			}
			else
			{
				// Leerzeile
				if($l && !$ldisp)
				{
					fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
					$l++;
				}

				// Rest (einmal über gesamte Zeile)
				if(!$ldisp)
				{
					$ldisp=true;

					$txt='<div class="s'.$fsz.'b" style="float:left;color:#ffffff;text-shadow:2px 2px 2px #000000;">&nbsp;&nbsp;'.meldung(2542, false).'</div>';
					$txt='<div class="s'.$fsz.'b tshadowdark" style="float:left;color:#'.$col.';">'.meldung(2542, false).'</div>';
					fxf_listSectHeader($mc, $l, $txt, $fcoll, $lcoll, $colspan, -2, 1, $bcol, 'padding:'.$pad.'px;');
					$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon']='~|';
					$l++;
				}

				// Name
				$palias=$ppa[$id]['lwname'];
			}

			$col='^class_rv'.(($l%2)+1).'|~';	// Standard
			// Nicht bestätigte Projektmitteilungen gelb darstellen
			if(($lw['terminart'] == 1131) && !$lw['hat_bestaetigt'])
				$col='^class_yat'.($l%2).'|~';	// gelb
			// Zu verschiebende Termine rosa darstellen
			else if($lw['verschieben'])
				$col='^class_nkz'.($l%2).'|~';	// rosa
			// Aktuellen Tag grün darstellen
			else if(substr($lw['von'], 0, 8) == substr($GLOBALS['datetime'], 0, 8))
				$col='^class_rg'.(($l%2)+1).'|~';	// grün

			// Name
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['person_alias']=$col.'~'.$palias;

			// Kalendericon -> Link auf Termin- bzw. Abwesenheitsplaner bzw. Projekt
			if($lw['terminart'] == FXP_AT_PROJECTMESSAGE)
			{
				$lnk=23;		// -> Vorgänge (Detail)
				$tt=$lw['tabwert'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon']='^class_transparent|~'.fieldSubmitImage('kalender['.$mc.']['.$l.']','icln'.$lnc,'',$tt,false,false,$lnk,'aktion=3&_projektid='.$lw['projekt_id'].'&Projekt_IDopen='.$lw['projekt_id'].'&keep_get=1');
			}
			else if($lw['zeit_id'] && $lw['projekt_id'] && (substr($lw['anmerkung'], 0, 2) != '0-') && $lw['erledigt'] && ($id == $GLOBALS['fxpglobals']['person']) && ($lw['endzeit'] < $jetzt))
			{
				$lnk=81;		// -> Zeiterfassung
				$tid=0;
				if($lw['zeit_id'] > 0)
					$tid=(int)db_value("SELECT taetigkeit_id FROM taetigkeiten WHERE zeit_id=".$lw['zeit_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				if($lw['projekt_id'] == -1)
					$zp=0;
				else
					$zp=$lw['projekt_id'];
				$tt=$lw['tabwert'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon']='^class_transparent|~'.fieldSubmitImage('kalender['.$mc.']['.$l.']','icln'.$lnc,'',$tt,false,false,$lnk,'aktion=3&tid='.$tid.'&gd='.$lw['startzeit'].'&ge='.$lw['endzeit'].'&persid='.$GLOBALS['fxpglobals']['person'].'&project='.$zp.'&keep_get=1');
			}
			else
			{
				$lnk=27;		// -> Terminplaner
				if($ist_fehltagsantrag || $ist_fehltag)
					$lnk=222;	// -> Abwesenheitsplaner

				if($lw['terminart'] == FXP_AT_REMINDER)
					$trmid=$lw['mastertermin_id'];
				else
					$trmid=$lw['termin_id'];
				$tt=$lw['tabwert'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['icon']='^class_transparent|~'.fieldSubmitImage('kalender['.$mc.']['.$l.']','icln'.$lnc,'',$tt,false,false,$lnk,'start=1&Termin_ID='.$trmid.'&aktion=3&keep_get=1');
			}

			// Zeitraum / Zeit
			if(($lw['terminart'] == FXP_AT_PROJECTMESSAGE) && $prjdata)
			{
				$tag=fx_date('w', fx_mktime(0, 0, 0, substr($lw['von'], 4, 2), substr($lw['von'], 6, 2), substr($lw['von'], 0, 4)));
				if(($tag < 1) || ($tag > 5))
					$tcls='ffffff';
				else
					$tcls='eeeeee';
				$z_dtm=get_entry($lw['von'], 'datum');

				// Zeitraum
				$var4=no_null($prjdata['vorgangsnummer']).' '.$prjdata['name_projekt'];
				$z_zeitraum='<table border=0 cellpadding=0 cellspacing=0><tr><td><div class="s1" style="background:#'.$tcls.';padding:2px;width:10px;border:1px solid #cccccc;border-radius:4px;text-align:center;">'.$wochentage[$tag].'</div></td><td>&nbsp;'.$z_dtm.':&nbsp;'.$var4.'&nbsp;</td></tr></table>';
				$var4=$z_dtm.': '.$var4;

				// Zeit
				if($prjdata['aufwand_ist'])
					$z_zsp=get_entry($prjdata['aufwand_ist'], 'zeitspannet');
				else
					$z_zsp='0:00';
				$var2=$z_zsp.' h';
				$z_zeit=$z_zsp.'&nbsp;<font class="grey">h</font>';
			}
			else if(($lw['terminart'] == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($lw['terminart'] == FXP_AT_OVERTIMEOUTPAY))
			{
				$tag=fx_date('w', fx_mktime(0, 0, 0, substr($lw['von'], 4, 2), substr($lw['von'], 6, 2), substr($lw['von'], 0, 4)));
				if(($tag < 1) || ($tag > 5))
					$tcls='ffffff';
				else
					$tcls='eeeeee';
				$z_dtm=get_entry($lw['von'], 'datum');

				// Zeitraum
				$z_zeitraum='<table border=0 cellpadding=0 cellspacing=0><tr><td><div class="s1" style="background:#'.$tcls.';padding:2px;width:10px;border:1px solid #cccccc;border-radius:4px;text-align:center;">'.$wochentage[$tag].'</div></td><td nowrap>&nbsp;'.$z_dtm.'&nbsp;</td></tr></table>';
				$var4=meldung(17+(int)substr($lw['von'], 4, 2), false).'&nbsp;'.substr($lw['von'], 0, 4);

				// Zeit
				$var2=get_entry($lw['stunden'], 'dezimal');
				$z_zeit=$var2.'&nbsp;<font class="grey">h</font>';

			}
			else if(substr($lw['von'], 0, 8) != substr($lw['bis'], 0, 8))
			{
				$tag1=fx_date('w', fx_mktime(0, 0, 0, substr($lw['von'], 4, 2), substr($lw['von'], 6, 2), substr($lw['von'], 0, 4)));
				if(($tag1 < 1) || ($tag1 > 5))
					$tcls1='ffffff';
				else
					$tcls1='eeeeee';
				$z_dtm1=get_entry($lw['von'],'datum');
				$z_von=get_entry($lw['von'],'zeit');

				$tag2=fx_date('w', fx_mktime(0, 0, 0, substr($lw['bis'], 4, 2), substr($lw['bis'], 6, 2), substr($lw['bis'], 0, 4)));
				if(($tag2 < 1) || ($tag2 > 5))
					$tcls2='ffffff';
				else
					$tcls2='eeeeee';
				$z_dtm2=get_entry($lw['bis'],'datum');
				$z_bis=get_entry($lw['bis'],'zeit');

				// Zeitraum
				$var4=$z_dtm1.', '.$z_von.' - '.$z_dtm2.', '.$z_bis;
				$z_zeitraum ='<table border=0 cellpadding=0 cellspacing=0><tr>';
				$z_zeitraum .= '<td><div class="s1" style="background:#'.$tcls1.';padding:2px;width:10px;border:1px solid #cccccc;border-radius:4px;text-align:center;">'.$wochentage[$tag1].'</div></td>';
				$z_zeitraum .= '<td nowrap>&nbsp;'.$z_dtm1.'&nbsp;<font class="grey">'.$z_von.'</font>&nbsp;<font class="lightgrey">-</font>&nbsp;</td>';
				$z_zeitraum .= '<td><div class="s1" style="background:#'.$tcls2.';padding:2px;width:10px;border:1px solid #cccccc;border-radius:4px;text-align:center;">'.$wochentage[$tag2].'</div></td>';
				$z_zeitraum .= '<td nowrap>&nbsp;'.$z_dtm2.'&nbsp;<font class="grey">'.$z_bis.'</font>&nbsp;</td>';
				$z_zeitraum .= '</tr></table>';

				// Zeit
				$z_tag=get_entry((float)$lw['urlaub'], 'dezimal');
				$var2=$z_tag.' d';
				$z_zeit=$z_tag.'&nbsp;<font class="grey">d</font>';
			}
			else
			{
				$tag=fx_date('w', fx_mktime(0, 0, 0, substr($lw['von'], 4, 2), substr($lw['von'], 6, 2), substr($lw['von'], 0, 4)));
				if(($tag < 1) || ($tag > 5))
					$tcls='ffffff';
				else
					$tcls='eeeeee';
				$z_dtm=get_entry($lw['von'],'datum');
				$z_von=get_entry($lw['von'],'zeit');
				$z_bis=get_entry($lw['bis'],'zeit');

				// Zeitraum
				$var4=$z_dtm.', '.$z_von;
				$z_zeitraum='<table border=0 cellpadding=0 cellspacing=0><tr><td><div class="s1" style="background:#'.$tcls.';padding:2px;width:10px;border:1px solid #cccccc;border-radius:4px;text-align:center;">'.$wochentage[$tag].'</div></td><td nowrap>&nbsp;'.$z_dtm.'&nbsp;<font class="grey">'.$z_von;
				if($lw['bis'] != $lw['von'])
				{
					$var4 .= '-'.$z_bis;
					$z_zeitraum .= '-'.$z_bis;
				}
				$z_zeitraum .= '</font></td></tr></table>';

				// Zeit
				if($lnk == '222')
				{
					$z_tag=get_entry((float)$lw['urlaub'], 'dezimal');
					$var2=$z_tag.' d';
					$z_zeit=$z_tag.'&nbsp;<font class="grey">d</font>';
				}
				else
				{
					$zd=validate_entry(time_diff($lw['von'], $lw['bis'], 'H'), $zerr, 'zeitspanne');
					if($zd)
						$z_zsp=get_entry($zd, 'zeitspannet');
					else if($lw['stunden'])
						$z_zsp=get_entry($lw['stunden'], 'dezimal');
					else
						$z_zsp='0:00';
					$var2=$z_zsp.' h';
					$z_zeit=$z_zsp.'&nbsp;<font class="grey">h</font>';
				}
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['zeitraum']=$col.$z_zeitraum;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['tage']=$col.$z_zeit;

			// Terminart
			if(!isAbsentApprovalAppointment($lw['terminart']) && !isAbsentAppointment($lw['terminart']) && strlen($lw['name_termin']))
				$var1=trim($lw['name_termin']);
			else
				$var1=$lw['tabwert'];
			if(fxIsArray($prjdata) || ($lw['projekt_id'] == -1))
			{
				if(fxIsArray($prjdata))
					$pri=no_null($prjdata['vorgangsnummer']);
				else
					$pri=meldung(681);	// Unbekannt
				$tdisp='<font class="lightgrey">&rarr;</font>&nbsp;<span style="padding:1px 2px 3px 3px;border:1px solid #cccccc;border-radius:4px;text-align:center;"><img src="'.$GLOBALS['gfxpath'].'c_task.png" align="top">&nbsp;'.$pri.'</span>&nbsp;&nbsp;'.$var1;
			}
			else
				$tdisp=$var1;
			if($lw['zeit_id'] > 0)
				$TerminIcon='c_time';
			else if(($lw['termin_id'] != $lw['mastertermin_id']) && $lw['wiederholung'])
				$TerminIcon='c_rept';
			else
				$TerminIcon=$TerminGrafiken[$lw['terminart']];
			if(!$TerminIcon)
				$TerminIcon='c_outl';
			$TerminIcon=$GLOBALS['gfxpath'].$TerminIcon.'.png';
			$link=link_img($TerminIcon, termin_tooltip($lw, $prjdata), 16, 16, 0);
			$ics_filename=iCalendar($lw['termin_id']);
			if(strlen($ics_filename))
				$tdisp .= '&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<a href="'.$ics_filename.'" style="position:absolute;right:0;"><img src="'.$GLOBALS['gfxpath'].'iccl.png" tooltip="'.$icta[0].'<br ><br /><i class=blue>'.$GLOBALS['mail_attachments'][$ics_filename]['summary'].'</i>" align="right" style="position:relative;cursor:pointer;"></a>';
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['tabwert']=$col.'<table width=100% border=0 cellpadding=0 cellspacing=2><tr><td valign=top>'.$link.'</td><td width=100% style="position:relative;">&nbsp;'.$tdisp.'</td></tr></table>';

			$anm=substr($lw['anmerkung'], 0, 1);

			if(!isset($has_supervisor[$id]))
				$has_supervisor[$id]=get_realelter($id);
//fxDebug($has_supervisor,'$has_supervisor');

			// Gen./Abl.
			$darf_gen=false;
			// Projektmitteilungen dürfen nur vom PL bzw. stellv. PL genehmigt werden
			if($lw['terminart'] == FXP_AT_PROJECTMESSAGE)
			{
				if($lw['projekt_id'])
				{
					$pls=db_values("SELECT projektleiter,vertreter FROM projekte WHERE projekt_id=".$lw['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
					if(($pls['projektleiter'] == $GLOBALS['fxpglobals']['person']) || ($pls['vertreter'] && ($pls['vertreter'] == $GLOBALS['fxpglobals']['person'])))
						$darf_gen=true;
				}
			}
			// Einladungen, ausser selbst erstellte, dürfen immer genehmigt werden (Ausnahmen weiter unten)
			else if($invitation && ($lw['person_angelegt'] != $GLOBALS['fxpglobals']['person']))
				$darf_gen=true;
			// GF darf alles genehmigen
			else if($GLOBALS['fxpglobals']['persdat']['roles'][2])
				$darf_gen=true;
			// Ansonsten dürfen Termine von Untergeordneten genehmigt werden
			else if(sizeof($all_personal) && $all_personal[$id])
				$darf_gen=true;
			// VW darf Termine genehmigen für Personen ohne Vorgesetzen oder für alle, falls mandantenweit so eingestellt, ausser für sich selbst
			else if($GLOBALS['fxpglobals']['persdat']['roles'][-1] && (($ap_can_appr_all && ($id != $GLOBALS['fxpglobals']['person'])) || !$has_supervisor[$id]))
				$darf_gen=true;
			if($darf_gen)
			{
				// Reine Mitteilungen oder Abwesenheitstage dürfen nie genehmigt werden
				if($ist_fehltag || ($lw['terminart'] == FXP_AT_MESSAGE))
					$darf_gen=false;
				// oder bereits erledigte Termine, ausser zukünftige Einladungen
				else if($lw['erledigt'] && (!$invitation || ($lw['startzeit'] < $GLOBALS['datetime'])))
					$darf_gen=false;
			}

			// Erl.
			$darf_erl=false;
			if($darf_gen)
			{
				if($err)
					$cval=$_POST['genehmigen'][$mc][$l];
				else if($anm === '1')
					$cval=2291;
				else if($anm === '0')
					$cval=2292;
				else
					$cval=0;
				$value_array=array(2291=>'',2292=>'', 0=>'');
				$tooltip_array=array(2291=>$gal74,2292=>$gal75, 0=>$gal76);
				if(($anm === '0') || ($anm === '1'))
					$value_array[0]='{disabled}';
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['genehmigen']=$col.'&nbsp;'.fieldRadio('genehmigen['.$mc.']['.$l.']',$cval,$value_array,'h',0,false,false,$tooltip_array);
			}
			else if($anm === '1')
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['genehmigen']=$col.'<img id="imgappr'.$l.'" src="'.$GLOBALS['gfxpath'].'tl_green.png" tooltip="'.substr($lw['anmerkung'],2).'" style="'.$glstyle.'cursor:help;"><img src="'.$GLOBALS['gfxpath'].'tl_neutral.png" style="'.$gmstyle.'"><img src="'.$GLOBALS['gfxpath'].'tl_neutral.png" style="'.$grstyle.'">';
				$darf_erl=true;
			}
			else if($anm === '0')
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['genehmigen']=$col.'<img src="'.$GLOBALS['gfxpath'].'tl_neutral.png" style="'.$glstyle.'"><img id="imgdecl'.$l.'" src="'.$GLOBALS['gfxpath'].'tl_red.png" tooltip="'.substr($lw['anmerkung'],2).'" style="'.$gmstyle.'cursor:help;"><img src="'.$GLOBALS['gfxpath'].'tl_neutral.png" style="'.$grstyle.'">';
				$darf_erl=true;
			}
			else
			{
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['genehmigen']=$col.'&nbsp;';
				if(!$invitation && !$ist_fehltagsantrag && ($id == $GLOBALS['fxpglobals']['person']))
					$darf_erl=true;
			}

			// erledigt -> nur bei eigenen Terminen, ansonsten ausblenden
			$done_st='';
			$done_tt='';
			if(!$darf_erl && ($anm === '0') || ($anm === '1'))
			{
				$done_tt=' tooltip="'.substr($lw['anmerkung'],2).'"';
				$done_st='cursor:help;';
			}
			if($lw['erledigt'])
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['erledigt']=$col.'<img src="'.$GLOBALS['gfxpath'].'1.png"'.$done_tt.' style="margin-top:2px;'.$done_st.'">';
			else if($darf_erl)
			{
				$chk='';
				if(($err && $_POST['erledigt'][$mc][$l]) || (!$err && $lw['erledigt']))
					$chk='1';
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['erledigt']=$col.fieldCheckbox('erledigt['.$mc.']['.$l.']','',$chk);
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$l]['erledigt']=$col.'<img src="'.$GLOBALS['gfxpath'].'0.png"'.$done_tt.' style="margin-top:3px;'.$done_st.'">';

			// Restliche Listenwerte füllen
			if(($lw['terminart'] == FXP_AT_OVERTIMEOUTPAYREQUEST) || ($lw['terminart'] == FXP_AT_OVERTIMEOUTPAY))
			{
				// 1946: <FONT COLOR="#000088">$var1</FONT> über $var2 h von <b>$var3</b> für <I>$var4</I>
				$ptxt=meldung(1946, false, $var1, $var2, $var3, $var4);
			}
			else
			{
				// 78: <FONT COLOR="#000088">$var1</FONT> über $var2 von <b>$var3</b> im Zeitraum <I>$var4</I>
				$ptxt=meldung(78, false, $var1, $var2, $var3, $var4);
			}
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['pname']=$ptxt;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['personen_id']=$id;
			$GLOBALS['fxptdata']['lvalues'][$mc][$l]['termin_id']=$lw['termin_id'];

			$l++;
		}
	}

	// Letzte Zeile
	if($l)
	{
		fxf_maskColumnBorder($fa, 1, -1, $mc, $l, true);
		$l++;
	}

	// Termine jeder Person
	if(sizeof($icpa))
	{
		foreach($icpa as $pattern => $hl)
		{
			$ica=iCalendarCombine(0,$pattern);
			if($ica['counter'] > 1)
			{
				$icl='<a href="'.$ica['file'].'" style="position:absolute;right:15px;"><img src="'.$GLOBALS['gfxpath'].'iccl.png" tooltip="'.$icta[1].'<br ><br /><i class=blue>'.$ica['names'].'</i>"></a>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$hl]['zeitraum'] .= $icl;
				$GLOBALS['fxptdata']['lvalues'][$mc][$hl]['erledigt'] .= $icl;
			}
		}
	}

	// Alle Termine
	$ica=iCalendarCombine(2);

	if(($no_person == 1) || $sort_person)
		$Person_alias_nv=true;
}
else
	$GLOBALS['fxptdata']['lvalues'][$mc]=true;

// Diese Felder editierbar machen
$genehmigen_sf=true;
$erledigt_sf=true;

$GLOBALS['fxptdata']['backup']['Heute_a']=$Heute_a;
$GLOBALS['fxptdata']['backup']['Heute_e']=$Heute_e;

// Speicherbutton anzeigen
$Button_okay_nv=true;
$button=fieldSubmit('Button_Speichern', get_text(1360, '20'), 'sub', 0, false, false, true);

// Maske anzeigen
$msk_html=maske(185);
echo($msk_html);

$maske185_nv=true;
?>