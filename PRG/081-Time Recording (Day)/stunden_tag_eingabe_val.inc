<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_tag_eingabe_val.inc                                  //
// Version     : 24.1                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2025-04-16                                                   //
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
 * PF 81: Time Recording (Day)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$date_error=false;
$Old_Projekt=array();

if(!$err && $db_update && $Button_OK && ($Dummy < FXP_DELETE))
{
	// Personentyp
	$perstyp=(int)db_value("SELECT personentyp FROM personen WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	//Zeitbeschränkungen für Int. Mitarbeiter?
	$zeitbeschr=false;
	if($perstyp == FXP_PT_EMPLOYEE)
	{
		$sql="SELECT zeitbeschraenkung FROM madaten WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$zbv=db_value($sql);
		if(($zbv === NULL) || !strlen((string)$zbv) || $zbv)
			$zeitbeschr=true;
	}
//fxDebug($perstyp,'$perstyp', 0); fxDebug($zeitbeschr,'$zeitbeschr', 0);

	if($sum)
		$val_maskennummer=220;
	else
		$val_maskennummer=218;

	// Prüfen, ob bereits Abwesenheitstag - falls ja, dann Warnhinweis anzeigen
	$trok=true;
	$ad=okTimeRegistration(0,$vb_pid,$datum);
	if(strlen((string)$ad))
	{
		$trok=false;
		error_msg(3014, array('Datum'), $ad);	// Ganztägige Abwesenheitstage an folgenden Tagen vorhanden:<br>$var1<br><br><font class=red>Es dürfen an diesen Tagen keine Zeiten erfasst werden!</font>
		$err=true;
	}

	// Genehmigungszeitraum ermitteln
	$this_pid	= 0;
	$gen_beginn	= '';
	$gen_ende	= '';
	$pln_beginn	= '';
	$pln_ende	= '';
	if($Projekt_ID)
	{
		$this_pid=(int)$Projekt_ID;

		// Genehmigungs- und Planzeitraum
		$sql="SELECT gen_beginn, gen_ende, soll_beginn, soll_ende FROM zeitdaten WHERE projekt_id=".$this_pid." AND zeitart=102 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$gpdata=db_values($sql);
		if(fxIsArray($gpdata))
		{
			if(($gpdata['gen_beginn'] !== NULL) && strlen((string)$gpdata['gen_beginn']))
				$gen_beginn=substr((string)$gpdata['gen_beginn'], 0,8);
			if(($gpdata['gen_ende'] !== NULL) && strlen((string)$gpdata['gen_ende']))
				$gen_ende=substr((string)$gpdata['gen_ende'], 0,8);
			if(($gpdata['soll_beginn'] !== NULL) && strlen((string)$gpdata['soll_beginn']))
				$pln_beginn=substr((string)$gpdata['soll_beginn'], 0,8);
			if(($gpdata['soll_ende'] !== NULL) && strlen((string)$gpdata['soll_ende']))
				$pln_ende=substr((string)$gpdata['soll_ende'], 0,8);
		}
	}

	// Start- und Enddatum
	if($sum)
	{
		if($_POST['startdatum'])
			$HTTP_startdatum=$_POST['startdatum'];
		else if($startdatum)
			$HTTP_startdatum=$startdatum;
		if(!strlen((string)$HTTP_startdatum))
		{
			// 752: Das Feld "Datum" darf nicht leer sein. Bitte geben Sie ein Datum ein!
			if($trok)
				error_msg(752,false,'startdatum');
			$date_error=true;
			$startdatum='';
		}
		else
		{
			$startdatum=validate_entry($HTTP_startdatum,$verr,'datum');
			if($verr)
			{
				if($trok)
					error_msg($verr,false,'startdatum');
				$date_error=$verr;
			}
		}
		if($date_error)
		{
			$datum=$GLOBALS['datetime'];
			$gd=$datum;
			$err=true;
		}
		else
			$gd=$startdatum;

		if($_POST['enddatum'])
			$HTTP_enddatum=$_POST['enddatum'];
		else if($enddatum)
			$HTTP_enddatum=$enddatum;
		$enddatum=validate_entry($HTTP_enddatum,$verr,'datum');
		if($verr)
		{
			if($trok)
				error_msg($verr,false,'enddatum');
			$date_error=$verr;
		}
		if($date_error)
			$err=true;

		if(!$date_error && strlen((string)$enddatum) && (substr((string)$enddatum,0,8) < substr((string)$startdatum,0,8)))
		{
			// 1990: Das Enddatum [$var1] kann nicht vor dem Startdatum [$var2] liegen!
			if($trok)
				error_msg(1990,array('enddatum'),get_entry($enddatum,'datum'),get_entry($startdatum,'datum'));
			$date_error=true;
			$err=true;
		}

		// Zeiterfassung ausserhalb des Arbeitsvertragzeitraums?
		if(!$date_error)
		{
			$sd8=substr((string)$startdatum,0,8);
			$ed8=substr((string)$enddatum,0,8);
			if(!strlen((string)$ed8))
			{
				$ed8=$sd8;
				$earr=array('startdatum');
			}
			else
				$earr=array('startdatum', 'enddatum');
			if(($sd8 < $GLOBALS['pers_entry_date']) || ($sd8 > $GLOBALS['pers_exit_date']) || ($ed8 < $GLOBALS['pers_entry_date']) || ($ed8 > $GLOBALS['pers_exit_date']))
			{
				// Keine Zeiterfassung von &quot;$var1&quot; möglich für den $var2, da ausserhalb des Arbeitsvertragzeitraums.
				$dtxt=get_entry($sd8,'datum');
				if($ed8 != $sd8)
					$dtxt .= '-'.get_entry($ed8,'datum');
				error_msg(3279, $earr, '<b>'.$GLOBALS['pers_name2'].'</b>', '<b>'.$dtxt.'</b>');
				$date_error=true;
				$err=true;
			}
		}
	}
	// Datum
	else
	{
		if($_POST['Datum'])
			$HTTP_Datum=$_POST['Datum'];
		else if($Datum)
			$HTTP_Datum=$Datum;
//echo('$HTTP_Datum='.$HTTP_Datum.'<br />');
		if(!strlen((string)$HTTP_Datum))
		{
			// 752: Das Feld "Datum" darf nicht leer sein. Bitte geben Sie ein Datum ein!
			if($trok)
				error_msg(752,false,'Datum');
			$date_error=true;
			$Datum='';
		}
		else
		{
			$datum=validate_entry($HTTP_Datum,$verr,'datum');
			if($verr)
			{
				if($trok)
					error_msg($verr,false,'Datum');
				$date_error=$verr;
			}
			$Datum=$datum;
		}
		if($date_error)
		{
			$datum=$GLOBALS['datetime'];
			$err=true;
		}
		$gd=$datum;
//echo('$datum='.$datum.'<br />');

		// Zeiterfassung ausserhalb des Arbeitsvertragzeitraums?
		if(!$date_error)
		{
			$d8=substr((string)$datum,0,8);
			if(($d8 < $GLOBALS['pers_entry_date']) || ($d8 > $GLOBALS['pers_exit_date']))
			{
				// Keine Zeiterfassung von &quot;$var1&quot; möglich für den $var2, da ausserhalb des Arbeitsvertragzeitraums.
				error_msg(3279, array('Datum'), '<b>'.$GLOBALS['pers_name2'].'</b>', '<b>'.get_entry($d8,'datum').'</b>');
				$date_error=true;
				$err=true;
			}
		}
	}

	// Gleitzeit
	if($this_pid == -1)
	{
		$wotag=chr(97+fx_date('w', fx_mktime(12, 0, 0, (int)substr((string)$datum, 4, 2), (int)substr((string)$datum, 6, 2), (int)substr((string)$datum, 0, 4))));
		$gsql  = "SELECT wochenarbeitstd, arbeitstage, soll_beginn, pause, montag AS b, dienstag AS c, mittwoch AS d, donnerstag AS e, freitag AS f, samstag AS g, sonntag AS a";
		$gsql .= " FROM madaten WHERE personen_ID=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$gval=db_values($gsql);

		$_POST['Kunde']=0;				// Kein Kunde
		$_POST['Ist_Verzoeg']='';
		$_POST['Zeiteinheit_Ist_Verz']=0;	// Keine Abweichung

		// Zeit
		$_POST['Ist_Beg_Zeit']=substr((string)$gval['soll_beginn'], -6, 2).':'.substr((string)$gval['soll_beginn'], -4, 2).':'.substr((string)$gval['soll_beginn'], -2);
		$_POST['Ist_End_Zeit']='';
		$_POST['Zeitaufwand']=$gval['wochenarbeitstd']/$gval['arbeitstage'];
		$_POST['Pause']=$gval['pause']/60;

		// Tätigkeit
		if(!strlen((string)$_POST['Taetigkeit_Text']))
			$_POST['Taetigkeit_Text']=$gleitzeit_str;	// [Gleitzeit]

		// 1667: Gleitzeit darf nur an einem Arbeitstag genommen werden
		if(!$gval[$wotag])
		{
			if($trok)
				error_msg(1667);
			$err=true;
		}

		extract($_POST);
	}

	// Abweichung
	if(!$sum && $Ist_Verzoeg)
	{
		if($Ist_Verzoeg && !$Zeiteinheit_Ist_Verz)
			$Zeiteinheit_Ist_Verz=856;	// Minuten, falls leer

		if($Zeiteinheit_Ist_Verz == 484)
			$Ist_Verzoeg_up=validate_entry($Ist_Verzoeg, $verr, 'dezimal')*3600;
		else if($Zeiteinheit_Ist_Verz == 485)
			$Ist_Verzoeg_up=validate_entry($Ist_Verzoeg, $verr, 'dezimal')*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
		else if($Zeiteinheit_Ist_Verz == 923)
			$Ist_Verzoeg_up=validate_entry($Ist_Verzoeg, $verr, 'dezimal')*$GLOBALS['fxpglobals']['settings']['mwoche']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
		else if($Zeiteinheit_Ist_Verz == 486)
			$Ist_Verzoeg_up=validate_entry($Ist_Verzoeg, $verr, 'dezimal')*$GLOBALS['fxpglobals']['settings']['mmonat']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
		else if($Zeiteinheit_Ist_Verz == 487)
			$Ist_Verzoeg_up=validate_entry($Ist_Verzoeg, $verr, 'dezimal')*$GLOBALS['fxpglobals']['settings']['mjahr']*$GLOBALS['fxpglobals']['settings']['mtag']*3600;
		else	// Für den Fall das Minuten (856) oder nichts gewählt wurde
			$Ist_Verzoeg_up=validate_entry($Ist_Verzoeg, $verr, 'dezimal')*60;

		if($verr)
		{
			$Ist_Verzoeg='^'.$Ist_Verzoeg;
			$err=true;
		}
	}
	else
		$Ist_Verzoeg_up=0;

	// Kunde
	if($dl_kunde)
		$Kunde=(int)$dl_kunde;
	else
		$Kunde=(int)$Kunde;
	if($z_kunde && !$Kunde && !$this_pid)
	{
		// 750: Das Feld $var1 muss ausgewählt werden!
		if($trok)
			error_msg(750, false, 'Kunde');
		$err=true;
	}
	// Prüfen, ob Auftraggeber zum Vorgang passt -> Gegebenenfalls ändern.
	if($this_pid > 0)	// Auftraggeber bei "Gleitzeit" und "Dummy" nicht prüfen
	{
		$pkunde=(int)db_value("SELECT kunde FROM projekte WHERE projekt_id=".$this_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if(!$Kunde)
			$Kunde=$pkunde;
		else if($Kunde != $pkunde)
		{
			$Kunde=$pkunde;

			// 1540: Der ausgewählte Auftraggeber wurde angepasst? da er nicht zum ausgewählten Vorgang zugeordnet war! Bitte bestätigen Sie die Änderung oder Ändern Sie den Vorgang.
			if($trok)
				error_msg(1540);
			$err=true;
		}
	}

	// Prüfen, ob Leistungsart angegeben werden muss
	if($kr_aktiv && $kr_lamuss && ($this_pid > 0) && !$lart_id)
	{
		// 805: Es <B>muss</B> eine Leistungsart statt einer Aufgabe ausgewählt werden!
		if($trok)
			error_msg(805, false, 'Projekt_ID');
		$err=true;
	}

	// Tätigkeit
	$HTTP_Taetigkeit_Text='';
	if(isset($_POST['Taetigkeit_Text']))
		$HTTP_Taetigkeit_Text=trim((string)$_POST['Taetigkeit_Text']);
	else if(isset($Taetigkeit_Text))
		$HTTP_Taetigkeit_Text=trim((string)$Taetigkeit_Text);
	if($z_taetigkeit && !strlen((string)$HTTP_Taetigkeit_Text))
	{
		// 749: Das Feld "Tätigkeit" darf nicht leer sein. Bitte geben Sie einen Text ein!
		if($trok)
			error_msg(749,false,'Taetigkeit_Text');
		$err=true;
	}

	// Pause (Standard=Minuten)
	if($_POST['Pause'])
		$HTTP_Pause=$_POST['Pause'];
	$verr4=false;	// Pause
	if(!$sum && $Pause)
	{
		$maskenfeldwerte['pause']=validate_entry($Pause, $verr4, 'zeitspannem');
		if(!$verr4)
		{
			if($maskenfeldwerte['pause'] > 86400)
				$maskenfeldwerte['pause']=86400;
			$Pause=get_entry($maskenfeldwerte['pause'],'zeitspannet');
			$zap=(int)(validate_entry($Pause, $verr, 'zeitspannem'));
		}
	}
	else
		$zap=0;
	$Pause=removeTags($Pause);

	// Zeitaufwand (Standard=Stunden)
	if(isset($_POST['Zeitaufwand']))
		$HTTP_Zeitaufwand=trim((string)$_POST['Zeitaufwand']);
	else if(isset($Zeitaufwand))
		$HTTP_Zeitaufwand=trim((string)$Zeitaufwand);
	$verr3=false;	// Zeitspanne
	if(strlen((string)$HTTP_Zeitaufwand))
	{
		$maskenfeldwerte['zeitaufwand']=validate_entry($HTTP_Zeitaufwand, $verr3, 'zeitspanne');
		if(!$verr3)
		{
			if(!$sum && ($maskenfeldwerte['zeitaufwand'] > 86400))
				$maskenfeldwerte['zeitaufwand']=86400;
			$Zeitaufwand=get_entry($maskenfeldwerte['zeitaufwand'],'zeitspannet');
			if(substr((string)$Zeitaufwand,0,1) == '-')
			{
				// 3001: Das Feld "$var1" muss größer 0 sein!
				if($trok)
					error_msg(3001,false,'Zeitaufwand');
				$err=true;
			}
		}
	}
	$Zeitaufwand=removeTags($Zeitaufwand);

	// Startzeit/Endzeit
	if($sum)
		$ztc=4;
	else
	{
		$ztc=0;
		if(!$verr3)
		{
			if(($Ist_Beg_Zeit !== '') && ($Ist_End_Zeit !== '') && ($Zeitaufwand === ''))
				$ztc=1;
			else if(($Ist_Beg_Zeit !== '') && ($Ist_End_Zeit === '') && ($Zeitaufwand !== ''))
				$ztc=2;
			else if(($Ist_Beg_Zeit === '') && ($Ist_End_Zeit !== '') && ($Zeitaufwand !== ''))
				$ztc=3;
			else if(($Ist_Beg_Zeit === '') && ($Ist_End_Zeit === '') && ($Zeitaufwand !== ''))
				$ztc=4;
		}
	}

//echo('<B>Ist_Beg_Zeit:</B> '.$Ist_Beg_Zeit.', <B>Ist_End_Zeit:</B> '.$Ist_End_Zeit.', <B>Zeitaufwand:</B> '.$Zeitaufwand.'<BR>');
//echo('<B>ztc:</B> '.$ztc.'<BR>');
	$verr1=false;	// Startzeit
	$verr2=false;	// Endzeit
	if($Ist_Beg_Zeit !== '')
	{
		$ibz=min(86400, (int)(validate_entry($Ist_Beg_Zeit, $verr1, 'zeitspanne')));
		$Ist_Beg_Zeit=get_entry($ibz, 'zeitspannet');
	}
	if($Ist_End_Zeit !== '')
	{
		$iez=min(86400, (int)(validate_entry($Ist_End_Zeit, $verr2, 'zeitspanne')));
		$Ist_End_Zeit=get_entry($iez, 'zeitspannet');
	}
//echo('<B>ibz:</B> '.$ibz.', <B>iez:</B> '.$iez.'<BR>');
	switch($ztc)
	{
		case 1:		// Startzeit + Endzeit
			$zeitcheck=true;
		break;

		case 2:		// Startzeit + Zeitaufwand
			$ibz=(int)(validate_entry($Ist_Beg_Zeit, $verr1, 'zeitspanne'));
			if($Zeitaufwand !== '')
			{
				$zs =(int)(validate_entry($Zeitaufwand, $verr3, 'zeitspanne'));
				$Ist_End_Zeit=get_entry(($ibz + $zs + $zap), 'zeitspannet');
				$Zeitaufwand='';
			}
			else
				$Ist_End_Zeit='';
			$zeitcheck=true;
		break;

		case 3:		// Endzeit + Zeitaufwand
			$iez=(int)(validate_entry($Ist_End_Zeit, $verr2, 'zeitspanne'));
			if($Zeitaufwand !== '')
			{
				$zs =(int)(validate_entry($Zeitaufwand, $verr3, 'zeitspanne'));
				$Ist_Beg_Zeit=get_entry(($iez - $zs - $zap), 'zeitspannet');
				$Zeitaufwand='';
			}
			else
				$Ist_Beg_Zeit='';
			$zeitcheck=true;
		break;

		case 4:	// Zeitaufwand
			if($z_zeit)	// Start- und Endzeit muss angegeben werden
				$zeitcheck=true;
			else
				$zeitcheck=false;
		break;
	}
//echo('<B>ibz:</B> '.$ibz.', <B>iez:</B> '.$iez.', <B>zs:</B> '.$zs.'<BR>');

	$fibz=false;
	$fiez=false;
	$fzaw=false;
	if(!$sum && $zeitcheck)
	{
		if($Ist_Beg_Zeit == '')
		{
			// 753: Das Feld "Startzeit" darf nicht leer sein. Bitte geben Sie eine Uhrzeit ein!
			if($trok)
				error_msg(753, false, 'Ist_Beg_Zeit');
			$fibz=true;
			$err=true;
		}
		else if(!$verr1)
			$dmy=validate_entry($Ist_Beg_Zeit, $verr1, 'zeitspanne');
		if($Ist_End_Zeit == '')
		{
			// 753: Das Feld "Endzeit" darf nicht leer sein. Bitte geben Sie eine Uhrzeit ein!
			if($trok)
				error_msg(753, false, 'Ist_End_Zeit');
			$fiez=true;
			$err=true;
		}
		else if(!$verr2)
			$dmy=validate_entry($Ist_End_Zeit, $verr2, 'zeitspanne');
	}
	else if($Zeitaufwand == '')
	{
		// 885: Das Feld "Zeitaufwand" darf nicht leer sein. Bitte geben Sie eine Zeitspanne ein!
		if($trok)
			error_msg(885, false, 'Zeitaufwand');
		$fzaw=true;
		$err=true;
	}

	if(!$sum)
	{
		if($verr1)
		{
			if($trok)
				error_msg($verr1, false, 'Ist_Beg_Zeit');
			$fibz=true;
			$err=true;
		}
		if($verr2)
		{
			if($trok)
				error_msg($verr2, false, 'Ist_End_Zeit');
			$fiez=true;
			$err=true;
		}
		if($verr4)
		{
			if($trok)
				error_msg($verr4, false, 'Pause');
			$err=true;
		}
	}
	if($verr3)
	{
		if($trok)
			error_msg($verr3, false, 'Zeitaufwand');
		$fzaw=true;
		$err=true;
	}

	if(!$sum && !$ztc)
	{
		$farr=array();
		if(!$fibz)
			$farr[]='Ist_Beg_Zeit';
		if(!$fiez)
			$farr[]='Ist_End_Zeit';
		if(!$fzaw)
			$farr[]='Zeitaufwand';

		if(sizeof($farr))
		{
//fxDebug($farr,'$farr');
			// 2222: Es müssen entweder die Start-/Endzeiten angegeben werden oder der Zeitaufwand!
			if($trok)
				error_msg(2222, $farr);
		}

		$err=true;
	}

	// Rel. Fertigstellungsgrad
	$fg_relativ='';
	if(isset($_POST['fg_relativ']))
		$HTTP_fg_relativ=trim((string)$_POST['fg_relativ']);
	else if(!isset($HTTP_fg_relativ))
		$HTTP_fg_relativ='';
	if($this_pid > 0)
	{
		$sql="SELECT methode, zeitpunkt, tage FROM projektfortschritt WHERE projekt_id=".$this_pid." AND (personen_id IS NULL OR personen_id=0) AND aktiv_kz=1 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$pfm=db_values($sql);
		if(!is_array($pfm))
			$pfm=array('methode'=>1784, 'zeitpunkt'=>1790, 'tage'=>0);
//fxDebug($pfm, '$pfm');

		if(!strlen((string)$HTTP_fg_relativ))
		{
			// 1826: Für diese Aufgabe <B>muss</B> in der Zeiterfassung ein Fertigstellungsgrad der Methode [<B>$var1</B>] eingegeben werden!
			if(($pfm['zeitpunkt'] == 1791) && ($pfm['methode'] != 1784) && !$fertig)
			{
				if($trok)
					error_msg(1826, array('fg_relativ'), meldung($pfm['methode'], true));
				$err=true;
			}
		}
		else
		{
			// 1827: Für diese Aufgaben kann kein Fertigstellungsgrad eingegeben werden, da Methode [<B>$var1</B>]. Die Eingabe wurde gelöscht!
			if($pfm['methode'] == 1784)
			{
				$fg_relativ				= '';
				$HTTP_fg_relativ		= '';
				$_POST['fg_relativ']	= '';

				if($trok)
					error_msg(1827, array('fg_relativ'), meldung($pfm['methode'], true));
				$err=true;
			}
			else
			{
				$fg_relativ=validate_entry($HTTP_fg_relativ, $verrfg, 'dezimal');
				if($verrfg)
				{
					if($trok)
						error_msg($verrfg, false, 'fg_relativ');
					$err=true;
				}
				else
				{
					$_POST['fg_relativ']='';
					switch($pfm['methode'])
					{
						case 1786:	// 0/50/100
							if(($fg_relativ != 0.00) && ($fg_relativ != 50.00) && ($fg_relativ != 100.00))
							{
								// 1828: Der eingegebene Fertigstellungsgrad von <B>$var1</B> entspricht nicht der Methode [<B>$var2</B>]. Der Wert wurde angepasst!
								if($trok)
									error_msg(1828, array('fg_relativ'), $fg_relativ, meldung($pfm['methode'], true));
								$err=true;

								if($fg_relativ < 0.00)
									$fg_relativ=0;
								else if($fg_relativ > 100.00)
									$fg_relativ=100;
								else
									$fg_relativ=50;
							}
							else
							{
								$fg_relativ=(int)$fg_relativ;
								if($fg_relativ == 100)
									$fertig=1;
							}
						break;

						case 1787:	// 0/100
							if(($fg_relativ != 0.00) && ($fg_relativ != 100.00))
							{
								// 1828: Der eingegebene Fertigstellungsgrad von <B>$var1</B> entspricht nicht der Methode [<B>$var2</B>]. Der Wert wurde angepasst!
								if($trok)
									error_msg(1828, array('fg_relativ'), $fg_relativ, meldung($pfm['methode'], true));
								$err=true;

								if($fg_relativ > 100.00)
									$fg_relativ=100;
								else
									$fg_relativ=0;
							}
							else
							{
								$fg_relativ=(int)$fg_relativ;
								if($fg_relativ == 100)
									$fertig=1;
							}
						break;

						case 1788:	// Prozent Restaufwand
						case 1789:	// Prozent Fertigstellung
							if(($fg_relativ < 0.00) || ($fg_relativ > 100.00))
							{
								// 1828: Der eingegebene Fertigstellungsgrad von <B>$var1</B> entspricht nicht der Methode [<B>$var2</B>]. Der Wert wurde angepasst!
								if($trok)
									error_msg(1828, array('fg_relativ'), $fg_relativ, meldung($pfm['methode'], true));
								$err=true;

								if($fg_relativ > 100.00)
									$fg_relativ=100;
								else
									$fg_relativ=0;
							}
							else
							{
								$fg_relativ=(int)$fg_relativ;
								if((($pfm['methode'] == 1788) && ($fg_relativ == 0)) || (($pfm['methode'] == 1789) && ($fg_relativ == 100)))
									$fertig=1;
							}
						break;

						default:	// Restaufwand (1785)
							if($fg_relativ < 0.00)
							{
								// 1828: Der eingegebene Fertigstellungsgrad von <B>$var1</B> entspricht nicht der Methode [<B>$var2</B>]. Der Wert wurde angepasst!
								if($trok)
									error_msg(1828, array('fg_relativ'), $fg_relativ, meldung($pfm['methode'], true));
								$err=true;

								$fg_relativ=0.00;
							}
							else if($fg_relativ == 0.00)
								$fertig=1;
							else
								$fg_relativ *= 3600.00;	// Umwandeln der Stunden in Sekunden
						break;
					}
//fxDebug($fg_relativ, '$fg_relativ');
				}
			}
		}

		// Fertigstellungsgrad anpassen, falls "abgeschlossen" angehakt wurde
		if(!$err && $fertig && ($pfm['methode'] != 1784) && !strlen((string)$fg_relativ))
		{
			switch($pfm['methode'])
			{
				case 1786:	// 0/50/100
				case 1787:	// 0/100
				case 1789:	// Prozent Fertigstellung
					$fg_relativ=100;
				break;

				case 1788:	// Prozent Restaufwand
					$fg_relativ=0;
				break;

				default:	// Restaufwand (1785)
					$fg_relativ=0.00;
				break;
			}
		}
	}
	// Für "unbekannte" Aufgaben kann kein Fertigstellungsgrad eingegeben werden. Die Eingabe wurde gelöscht!
	else if(strlen((string)$HTTP_fg_relativ))
	{
		$fg_relativ='';
		$HTTP_fg_relativ='';
		$_POST['fg_relativ']='';

		if($trok)
			error_msg(1825, array('fg_relativ'));
		$err=true;
	}

	// Prüfen, ob Zeiteintrag in Zukunft
	if($sum)
	{
		$dt8s=substr((string)$startdatum,0,8);
		if(strlen((string)$enddatum))
			$dt8e=substr((string)$enddatum,0,8);
		else
			$dt8e=$dt8s;
	}
	else
	{
		$dt8s=substr((string)$datum,0,8);
		$dt8e=$dt8s;
	}
	if((strtolower((string)$GLOBALS['fxpglobals']['settings']['zeit_zukunft']) != 'ja') && (strtolower((string)$GLOBALS['fxpglobals']['settings']['ze_alle_aktiv']) != 'ja') && ($dt8e > $heute))
	{
		// 1448: Es dürfen keine Zeiteinträge gemacht werden, die in der Zukunft liegen.
		if($trok)
			error_msg(1448,false);
		$err=true;
	}

	// Prüfen, ob Zeiteintrag im genehmigten Bereich
	if((strtolower((string)$GLOBALS['fxpglobals']['settings']['zd_in_genzeit']) == 'ja') && ((strlen((string)$gen_beginn) && ($dt8s < $gen_beginn)) || (strlen((string)$gen_ende) && ($dt8e > $gen_ende))))
	{
		if(strlen((string)$gen_beginn))
			$var1=get_entry($gen_beginn,'datum');
		else
			$var1='&infin;';
		$var1 .= ' - ';
		if(strlen((string)$gen_ende))
			$var1 .= get_entry($gen_ende,'datum');
		else
			$var1 .= '&infin;';

		// 1346: Dieser Eintrag liegt ausserhalb des genehmigten Zeitraums ($var1)!
		if($trok)
			error_msg(1346,array('Datum'),$var1);
		$err=true;
	}

	// Prüfen, ob Zeiteintrag im geplanten Bereich
	if((strtolower((string)$GLOBALS['fxpglobals']['settings']['zd_in_plnzeit']) == 'ja') && ((strlen((string)$pln_beginn) && ($dt8s < $pln_beginn)) || (strlen((string)$pln_ende) && ($dt8e > $pln_ende))))
	{
		if(strlen((string)$pln_beginn))
			$var1=get_entry($pln_beginn,'datum');
		else
			$var1='&infin;';
		$var1 .= ' - ';
		if(strlen((string)$pln_ende))
			$var1 .= get_entry($pln_ende,'datum');
		else
			$var1 .= '&infin;';

		// 2859: Dieser Eintrag liegt ausserhalb des geplanten Soll-Zeitraums ($var1)!
		if($trok)
			error_msg(2859,array('Datum'),$var1);
		$err=true;
	}

	if(!$err)
	{
		if($Zeit_ID)
			$Old_Projekt=db_values("SELECT projekt_id, mutterprojekt_id, zeitaufwand, kosten_ek, nicht_abrechenbar FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

		if(!$sum)
		{
			// Max. Arbeitsstunden pro Tag, pro Woche, Ruhepausen etc. holen (nur relevant für Zeitbeschränkung)
			if($zeitbeschr)
			{
				$maxstd_tag=$GLOBALS['fxpglobals']['settings']['maxstd_tag'];
				$maxstd_woche=$GLOBALS['fxpglobals']['settings']['maxstd_woche'];
				if(!$maxstd_tag || ($maxstd_tag <= 0))
					$maxstd_tag=24;
				if(!$maxstd_woche || ($maxstd_woche <= 0))
					$maxstd_woche=168;
			}
			else
			{
				$maxstd_tag=24;
				$maxstd_woche=168;
			}
//fxDebug($maxstd_tag,'$maxstd_tag', 0); fxDebug($maxstd_woche,'$maxstd_woche', 0);

			$arbzg_ruhepausen_sek_array=time_break($vb_pid);
//fxDebug($arbzg_ruhepausen_sek_array, '$arbzg_ruhepausen_sek_array');

			$akt_zeitpausen_sek=array();
			$fld_minpause=array('Pause');
			$akt_minpause_sek=86400;
			if($zap)
				$akt_minpause_sek=$zap;
			$akt_gesamtpausen_sek=$zap;
			if($zeitcheck)
			{
				// Prüfen, ob Endzeit nach Startzeit liegt
				$ibz=validate_entry($Ist_Beg_Zeit, $verr, 'zeit');
				if($Pause)
				{
					$p=validate_entry($Pause, $verr, 'zeit');
					$ibz=substr('00000000000000'.(string)((int)$ibz + (int)$p), -14);
				}
				$iez=validate_entry($Ist_End_Zeit, $verr, 'zeit');
				if($iez == '00000000000000')
					$iez='00000000240000';
				if($ibz >= $iez)
				{
					// 931: Startzeit inklusive Pause ist größer oder gleich der Endzeit
					error_msg(931,false);
					$err=true;
				}

				// Prüfen, ob Zeitdaten bereits belegt sind + Stunden add
				$zab=(int)(validate_entry($Ist_Beg_Zeit, $verr, 'zeitspanne'));
				$zae=(int)(validate_entry($Ist_End_Zeit, $verr, 'zeitspanne'));
				if(!$zae)
					$zae=86400;
				$zap=(int)(validate_entry($Pause, $verr, 'zeitspannem'));
				$zaw=$zae - $zap - $zab;

				$akt_zeitpausen_sek[$zab]=$zae;

				$cdatum=validate_entry($Datum, $verr, 'datum');
				$von=substr((string)$cdatum,0,8).'000000';
				$bis=substr((string)$cdatum,0,8).'240000';

				$csql  = "SELECT zeit_id, ist_beginn, ist_ende, zeitaufwand, pause";
				$csql .= " FROM zeitdaten";
				$csql .= " WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TRAVELEXP_TIMEREC.") AND ist_beginn>='".$von."' AND ist_ende<='".$bis."' AND zeitaufwand>0";
				$csql .= " ORDER BY ist_beginn";

				$zd=db_values($csql, '*');
				$zdok=true;
				$tages_zaw=$zaw;
				if(fxIsArray($zd))
				{
					$ibz=(int)(validate_entry($Ist_Beg_Zeit, $verr, 'zeit'));
					$iez=(int)$iez;
					foreach($zd as $zeiteintrag)
					{
						if(($Dummy > FXP_CREATE) && ($zeiteintrag['zeit_id'] == $Zeit_ID))
							continue;

						$tages_zaw += $zeiteintrag['zeitaufwand'];
						$akt_gesamtpausen_sek += $zeiteintrag['pause'];

						$zbz=(int)((int)(substr((string)$zeiteintrag['ist_beginn'], 8, 2)*3600) + (int)(substr((string)$zeiteintrag['ist_beginn'], 10, 2)*60) + (int)substr((string)$zeiteintrag['ist_beginn'], 12, 2));
						$zez=(int)((int)(substr((string)$zeiteintrag['ist_ende'], 8, 2)*3600) + (int)(substr((string)$zeiteintrag['ist_ende'], 10, 2)*60) + (int)substr((string)$zeiteintrag['ist_ende'], 12, 2));
						if($zez > $zbz)
							$akt_zeitpausen_sek[$zbz]=$zez;

						$zbz=(int)(substr((string)$zeiteintrag['ist_beginn'], 8));
						$zez=(int)(substr((string)$zeiteintrag['ist_ende'], 8));
						if($zbz || $zez)
						{
							if($zez == 0)
								$zez=240000;
							if(!(($ibz >= $zez) || ($iez <= $zbz)))
								$zdok=false;
						}
					}
				}

				if(!$zdok)
				{
					// 1205: Zeitüberlappung! In dieser Zeitspanne existieren bereits Einträge.
					error_msg(1205,false);
					$err=true;
				}
			}
			else
			{
				$zaw=(int)(validate_entry($Zeitaufwand, $verr, 'zeitspanne'));
				$zap=(int)(validate_entry($Pause, $verr, 'zeitspannem'));

				$cdatum=validate_entry($Datum, $verr, 'datum');
				$von=substr((string)$cdatum, 0, 8).'000000';
				$bis=substr((string)$cdatum, 0, 8).'240000';

				$csql  = "SELECT zeit_id, ist_beginn, ist_ende, zeitaufwand, pause";
				$csql .= " FROM zeitdaten";
				$csql .= " WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TRAVELEXP_TIMEREC.") AND ist_beginn>='".$von."' AND ist_ende<='".$bis."'";

				$zd=db_values($csql, '*');
				$tages_zaw=$zaw;
				if(fxIsArray($zd))
				{
					foreach($zd as $zeiteintrag)
					{
						if(($Dummy > FXP_CREATE) && ($zeiteintrag['zeit_id'] == $Zeit_ID))
							continue;

						$tages_zaw += $zeiteintrag['zeitaufwand'];
						$akt_gesamtpausen_sek += $zeiteintrag['pause'];

						$zbz=(int)((int)(substr((string)$zeiteintrag['ist_beginn'], 8, 2)*3600) + (int)(substr((string)$zeiteintrag['ist_beginn'], 10, 2)*60) + (int)substr((string)$zeiteintrag['ist_beginn'], 12, 2));
						$zez=(int)((int)(substr((string)$zeiteintrag['ist_ende'], 8, 2)*3600) + (int)(substr((string)$zeiteintrag['ist_ende'], 10, 2)*60) + (int)substr((string)$zeiteintrag['ist_ende'], 12, 2));
						if($zez > $zbz)
							$akt_zeitpausen_sek[$zbz]=$zez;
					}
				}
			}

			if(sizeof($akt_zeitpausen_sek) > 1)
			{
				ksort($akt_zeitpausen_sek);
//fxDebug($akt_zeitpausen_sek, '$akt_zeitpausen_sek');
				$cezr=0;
				foreach($akt_zeitpausen_sek as $cbz => $cez)
				{
					if($cezr > 0)
					{
						$cpa=$cbz-$cezr;
						if($cpa > 0)
						{
							if($cpa < $akt_minpause_sek)
							{
								$fld_minpause=array('Ist_Beg_Zeit', 'Ist_End_Zeit');
								$akt_minpause_sek=$cpa;
							}
							$akt_gesamtpausen_sek += $cpa;
						}
					}

					$cezr=$cez;
				}
			}

			// Prüfen, ob Arbeitsstunden pro Tag überschritten
			if($zeitbeschr)
			{
				$tages_zaw_h=$tages_zaw/3600;
				if($tages_zaw_h > $maxstd_tag)
				{
					$std_max=get_entry($maxstd_tag, 'dezimal');
					$std_diff=get_entry(($tages_zaw_h - $maxstd_tag), 'dezimal');

					// 1446: Stundenüberschreitung um $var1 Stunde(n)! Pro Tag sind max. $var2 Stunde(n) erlaubt.
					error_msg(1446, false, '<b>'.$std_diff.'</b>', $std_max);
					$err=true;
				}
			}

			// Prüfen, ob Arbeitsstunden pro Woche überschritten
			if($zeitbeschr)
			{
				$dow=fx_date('w', fx_mktime(0, 0, 0, substr((string)$cdatum, 4, 2), substr((string)$cdatum, 6, 2), substr((string)$cdatum, 0, 4))); // Tag der Woche ermitteln (0=Sonntag, 1=Montag, ..., 6=Samstag)
				$ewt=(int)($GLOBALS['fxpglobals']['settings']['ersterwt']) % 7; // Erster Wochentag (0=Sonntag, 1=Montag, ..., 6=Samstag)
				$tdiff=$dow-$ewt;
				if($tdiff < 0)
					$tdiff += 7;

				$wbeginn=fx_date('Ymd000000', fx_mktime(0, 0, 0, substr((string)$cdatum, 4, 2), substr((string)$cdatum, 6, 2) - $tdiff, substr((string)$cdatum, 0, 4)));
				$wende=fx_date('Ymd240000', fx_mktime(0, 0, 0, substr((string)$wbeginn, 4, 2), substr((string)$wbeginn, 6, 2) + 6, substr((string)$wbeginn, 0, 4)));
//fxDebug(array('$cdatum'=>$cdatum, '$dow'=>$dow, '$ewt'=>$ewt, '$tdiff'=>$tdiff, '$wbeginn'=>$wbeginn, '$wende'=>$wende),'Wochenstundendaten', 0);

				$csql  = "SELECT zeit_id, zeitaufwand";
				$csql .= " FROM zeitdaten";
				$csql .= " WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TRAVELEXP_TIMEREC.") AND ist_beginn<='".$wende."' AND ist_ende>='".$wbeginn."'";
				$zd=db_values($csql, '*');
				$wochen_zaw=$zaw;
//fxDebug($zd,'$wochen_zaw='.$wochen_zaw.'<br />'.$csql, 0);
				if(fxIsArray($zd))
				{
					foreach($zd as $zeiteintrag)
					{
						if(($Dummy > FXP_CREATE) && ($zeiteintrag['zeit_id'] == $Zeit_ID))
							continue;

						$wochen_zaw += $zeiteintrag['zeitaufwand'];
					}
				}

				$wochen_zaw_h=$wochen_zaw/3600;
//fxDebug($wochen_zaw_h,'$wochen_zaw_h: $maxstd_woche='.$maxstd_woche.', $wochen_zaw='.$wochen_zaw, 0);
				if($wochen_zaw_h > $maxstd_woche)
				{
					$std_max=get_entry($maxstd_woche, 'dezimal');
					$std_diff=get_entry(($wochen_zaw_h - $maxstd_woche), 'dezimal');

					// 1447: Stundenüberschreitung um $var1 Stunde(n)! Pro Woche sind max. $var2 Stunde(n) erlaubt.
					error_msg(1447, false, '<b>'.$std_diff.'</b>', $std_max);
					$err=true;
				}
			}

			// Prüfen, ob Mindestruhepause eingehalten wurde (z.B. falls ArbZG aktiviert, dann muss eine Pause mind. 15 min betragen)
			if($zeitbeschr && is_array($arbzg_ruhepausen_sek_array) && sizeof($arbzg_ruhepausen_sek_array) && ($akt_minpause_sek < $arbzg_ruhepausen_sek_array[0]))
			{
				// 1924: Eine Ruhepause muss mindestens $var1 Minuten betragen!
				error_msg(1924, $fld_minpause, '<b>'.get_entry($arbzg_ruhepausen_sek_array[0]/60, 'ganzzahl').'</b>');
				$err=true;
			}

			// Prüfen, ob Ruhepausen eingehalten wurde (z.B. falls ArbZG aktiviert, dann muss eine Pause von mind. 30 min nach 6 Stunden Arbeitszeit gemacht werden)
			if($zeitbeschr && is_array($arbzg_ruhepausen_sek_array) && (sizeof($arbzg_ruhepausen_sek_array) > 1))
			{
				$arbzg_arbeitszeit_sek_found=0;
				foreach($arbzg_ruhepausen_sek_array as $arbzg_arbeitszeit_sek => $arbzg_minpause_sek)
				{
					if($arbzg_arbeitszeit_sek && ($tages_zaw > $arbzg_arbeitszeit_sek))
						$arbzg_arbeitszeit_sek_found=$arbzg_arbeitszeit_sek;
				}

				if($arbzg_arbeitszeit_sek_found && ($akt_gesamtpausen_sek < $arbzg_ruhepausen_sek_array[$arbzg_arbeitszeit_sek_found]))
				{
					$akt_gesamtpausen_sek_diff=$arbzg_ruhepausen_sek_array[$arbzg_arbeitszeit_sek_found]-$akt_gesamtpausen_sek+$zap;

					// 1925: Die Summe der Ruhepausen für eine gesamte Tagesarbeitszeit von $var1 Stunden muss mindestens $var2 Minuten betragen, d.h. die eingetragene Pause muss mindestens $var3 Minute(n) betragen!
					error_msg(1925, array('Pause'), get_entry($tages_zaw/3600, 'dezimal'), get_entry($arbzg_ruhepausen_sek_array[$arbzg_arbeitszeit_sek_found]/60, 'ganzzahl'), '<b>'.get_entry($akt_gesamtpausen_sek_diff/60, 'ganzzahl').'</b>');
					$err=true;
				}
			}
		}
		else
		{
			$zaw=(int)(validate_entry($Zeitaufwand,$verr,'zeitspanne'));
			$zap=0;
		}

		// Prüfen, ob Probleme für die zugeordnete Aufgabe auftreten
		if($this_pid > 0)
		{
			$psql  = "SELECT p.aufwand_soll, p.aufwand_ist, p.max_zeit_aufw, p.deadline, z.soll_beginn, z.soll_ende";
			$psql .= " FROM projekte p, zeitdaten z WHERE z.projekt_id=p.projekt_id AND p.projekt_id=".$this_pid;
			$psql .= " AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pia=db_values($psql);
//fxDebug($pia, $psql);

			$p_heute		= $GLOBALS['datetime'];
			$p_aufwand_soll	= (double)$pia['aufwand_soll'];
			$p_aufwand_ist	= (double)$pia['aufwand_ist']+(double)$zaw;
			$p_aufwand_max	= (double)$pia['max_zeit_aufw'];
			$p_deadline		= (string)$pia['deadline'];
			$p_startdatum	= (int)substr((string)$pia['soll_beginn'],0,8);
			$p_aktdatum		= (int)substr((string)$datum,0,8);
			if(sizeof($Old_Projekt))
				$p_aufwand_ist -= (double)$Old_Projekt['zeitaufwand'];
//echo('<B>Alter gesamter Zeitaufwand Ist:</B> "'.$p_aufwand_ist.'"<BR>');

			$psql  = "SELECT SUM(zeitaufwand) AS aufwand_ist FROM zeitdaten WHERE projekt_id=".$this_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$psql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.") AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
			if($Zeit_ID)
			{
				$zad=db_values("SELECT zeitart,zeiteinheit FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
				$psql .= " AND zeit_id<>".$Zeit_ID;
				if(($zad['zeitart'] == FXP_TE_TIMEREC_SUM) && ($zad['zeiteinheit'] == $Zeit_ID))
					$psql .= " AND (zeiteinheit IS NULL OR zeiteinheit<>".$zad['zeiteinheit'].")";
			}
			$r_aufwand_ist=(double)db_value($psql);
//fxDebug($r_aufwand_ist, $psql);
//echo('<B>Alter abrechenbarer Zeitaufwand Ist:</B> "'.$r_aufwand_ist.'"<BR>');
			if(!$_POST['nicht_abrechenbar'])
				$r_aufwand_ist += (double)$zaw;
//echo('<B>Neuer abrechenbarer Zeitaufwand Ist:</B> "'.$r_aufwand_ist.'"<BR>');

			// FEHLER 1624: Für die zugeordnete Aufgabe (Startdatum: $var1) darf noch keine Zeiterfassung gebucht werden!
			if(!$err && (strtolower((string)$GLOBALS['fxpglobals']['settings']['ze_alle_aktiv']) != 'ja') && ($p_aktdatum < $p_startdatum))
			{
				error_msg(1624,false,get_entry($p_startdatum,'datum'));
				$err=true;
			}

			// FEHLER 1570: Stundenüberschreitung um $var1 Stunde(n)! Für die zugeordnete Aufgabe dürfen max. $var2 verrechenbare Stunde(n) eingegeben werden.
			if(!$err && $p_aufwand_max && ($r_aufwand_ist > $p_aufwand_max))
			{
				$var1=get_entry(($r_aufwand_ist-$p_aufwand_max),'zeitspannet');
				$var2=get_entry($p_aufwand_max,'zeitspannet');
				error_msg(1570,false,$var1,$var2);
				$err=true;
			}
			// WARNUNG 1569: Stundenüberschreitung um $var1 Stunde(n)! Für die zugeordnete Aufgabe wurden nur $var2 verrechenbare Stunde(n) veranschlagt.
			else if(!$err && $p_aufwand_soll && ($r_aufwand_ist > $p_aufwand_soll))
			{
				$var1=get_entry(($r_aufwand_ist-$p_aufwand_soll),'zeitspannet');
				$var2=get_entry($p_aufwand_soll,'zeitspannet');
				error_msg(1569,false,$var1,$var2);
			}

			// WARNUNG 1572: Achtung! Die Deadline ($var1) für die zugeordnete Aufgabe ist bereits um $var2 überschritten.
			if(!$err && $p_deadline && (substr((string)$p_heute,0,10) > substr((string)$p_deadline,0,10)))
			{
				$var1=get_entry($p_deadline,'datetime');
				$p_diff=time_diff($p_deadline,$p_heute,'H');
				if($p_diff < 48)	// alles unter 2 Tagen in Stunden anzeigen
				{
					$p_diff=get_entry(time_diff($p_deadline,$p_heute,'S'),'zeitspannet');
					$var2=$p_diff.' '.meldung(916,false);	// Stunde(n)
				}
				else
				{
					$p_diff=get_entry(time_diff($p_deadline,$p_heute,'D'),'dezimal');
					$var2=$p_diff.' '.meldung(917,false);	// Tag(e)
				}
				error_msg(1572,false,$var1,$var2);
			}
			// WARNUNG 1571: Achtung! Die Deadline für die zugeordnete Aufgabe läuft heute aus.
			else if(!$err && $p_deadline && (substr((string)$p_heute,0,8) == substr((string)$p_deadline,0,8)))
				error_msg(1571);
		}
	}

	if(!$zap)
		$zap='NULL';
}
?>