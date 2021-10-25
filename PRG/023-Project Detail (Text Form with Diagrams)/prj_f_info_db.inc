<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_f_info_db.inc                                            //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if(!$err && fxIsArray($_POST) && isset($_POST['Aufwand_Soll']) && (isset($_POST['Button_okay']) || isset($_POST['Button_Loeschen'])))
{
	$nl_repl=array("<br>\r\n"=>"<br>\r\n", "\r\n"=>"<br>\r\n", "\n"=>"<br>\n"); 
	$Projekt_ID=(int)$Projekt_ID;
	$da=array();

	if(!$Projektmanager)
	{
		if($vEPID)
			$Projektmanager=(int)db_value("SELECT projektmanager FROM projekte WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		else
			$Projektmanager=0;
	}

	// Bisherige Stand in $history
	if($Projekt_ID && !fxIsArray($history) && fxIsArray($pinfo) && fxIsArray($pinfo[$Projekt_ID]))
		$history=$pinfo[$Projekt_ID]['data'];

	// Ermitteln des Aufwands und des Budgets vor den Änderungen.
	// Wird erst am Ende der Datei verarbeitet, nachdem der DB_Zugriff bereits gelaufen ist
	$correction=false;
	if($vPART == FXP_PRJ_TASK)
	{
		// Korrektur standardmäßig ausführen
		$correction=true;

		if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
			$gap_aufwand=$Aufwand_Soll_up;
		else if($GLOBALS['fxpglobals']['action'] == FXP_DELETE)
			$gap_aufwand=-$history['aufwand_soll'];
		else if($history['aufwand_soll'] != $Aufwand_Soll_up)
			$gap_aufwand=$Aufwand_Soll_up - $history['aufwand_soll'];
		else
			$correction=false;
	}

	if(strlen($Ref_nummer))
		$refnr="'".convert_string($Ref_nummer, 'todb')."'";
	else
		$refnr='NULL';

	// Prefix
	$prefix='';
	if(isset($GLOBALS['pprefix']))
		$prefix=trim($GLOBALS['pprefix']);
	else if(isset($_POST['pprefix']))
		$prefix=trim($_POST['pprefix']);
	if(strlen($prefix))
	{
		$prefix=str_replace(' ', '', $prefix);
		if(strlen($prefix))
			$Name_Projekt=trim($prefix.' '.$Name_Projekt);
//fxDebug($prefix,'$prefix');
	}

	// Einsatzort
	$loc_col2=", port";
	$loc_val2=", NULL";
	$loc_colval3=", port=NULL";
	if(isset($_POST['port']) && strlen($_POST['port']))
	{
		$loc_val2=", '".convert_string($_POST['port'], 'todb')."'";
		$loc_colval3=", port='".convert_string($_POST['port'], 'todb')."'";
	}

	// Freigabe
	$app_col2="";
	$app_val2="";
	$app_colval3="";
	if($app)
	{
		$app_col2=", freigabe";
		$app_val2=", NULL";
		$app_colval3=", freigabe=NULL";
		if(isset($_POST['Freigabe']) && $_POST['Freigabe'])
		{
			$app_val2=", 1";
			$app_colval3=", freigabe=1";
		}
	}

	$special_conv_arr=array('Name_Projekt', 'Beschreibung');
	foreach($special_conv_arr as $special_conv)
	{
		if(!is_array($_already_converted[$special_conv]))
		{
			${$special_conv}=convert_string(${$special_conv}, 'todb');
			$_already_converted[$special_conv]=array('val'=>${$special_conv}, 'db'=>${$special_conv});
		}

		if(strlen(${$special_conv}))
			${$special_conv}="'".${$special_conv}."'";
		else
			${$special_conv}='NULL';
	}
//fxDebug($Name_Projekt,'$Name_Projekt');

	switch($GLOBALS['fxpglobals']['action'])
	{
		case FXP_CREATE:
			if($Projekt_ID < 1)	// Kann eigentlich nicht vorkommen, da bereits für Projektfortschritt ermittelt
			  	$Projekt_ID	= id_max('projekte', 'projekt_id');
			if($kr_aktiv)		// Neue ID für Kostenzuordnung übergeben (nur zur Sicherheit, da eigentlich ID umgekehrt ermittelt wird)
				$_POST['nv_text5']=$Projekt_ID;
	  		$Zeit_ID=id_max('zeitdaten', 'zeit_id');

			if($vPART == FXP_PRJ_MAIN)
			{
				$vMPID=$Projekt_ID;
				$vEPID=0;
			}
			else
				$vMPID=db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

			// Projekte
			if(!$err)
			{
				$query_proj  = "INSERT INTO projekte";
				$query_proj .= " (mandanten_id, projekt_id, mutterprojekt_id, budget_id, name_projekt, projektart,";
				$query_proj .= " kategorie_id, projektleiter, projektmanager, vertreter, ansprechpartner, kunde,";
				$query_proj .= " projekt_status, prioritaet, projekt_archiviert, abhaengigkeit, beschreibung, aufwand_soll, aufwand_todo,";
				$query_proj .= " aufwand_ist, elter, erstelldatum, zeit_id, deadline, max_zeit_aufw, anmerkung,";
				$query_proj .= " anmerkungintern, maart_id, mafaehigkeit_id, vorgangsnummer, ref_nummer".$loc_col2.$app_col2.")";
				$query_proj .= " VALUES";
				$query_proj .= " (".$GLOBALS['fxpglobals']['client'].", ".$Projekt_ID.", ".$vMPID.", 0, ".$Name_Projekt.", ".$vPART.",";
				$query_proj .= " ".$Kategorie_ID.", ".$Projektleiter.", ".$Projektmanager.", ".$Vertreter.", ".$Ansprechpartner.", ".$Kunde.",";
				$query_proj .= " ".$Projekt_Status.", ".$Prioritaet.", NULL, NULL, ".$Beschreibung.", ".$Aufwand_Soll_up.", 0,";
				$query_proj .= " 0, ".$vEPID.", '".$Erstelldatum."', ".$Zeit_ID.", ".$Deadline_up.", ".$Max_Zeit_Aufw_up.", ".$Anmerkung_up.",";
				$query_proj .= " ".$AnmerkungIntern_up.", ".$MaArt_ID.", ".$MaFaehigkeit_ID.", '".$Vorgangsnummer_new."', ".$refnr.$loc_val2.$app_val2.")";
				$err=db_x($query_proj);
fxDebug($err,$query_proj, 2);
			}

			// Zeitdaten
			if(!$err)
			{
				$query_zeit  = "INSERT INTO zeitdaten";
				$query_zeit .= " (mandanten_id, zuordnungs_mid, zeit_id, soll_beginn, soll_ende, gen_beginn, gen_ende, ist_verzoeg, zeitart, projekt_id, mutterprojekt_id)";
				$query_zeit .= " VALUES";
				$query_zeit .= " (".$GLOBALS['fxpglobals']['client'].", ".$GLOBALS['fxpglobals']['client'].", ".$Zeit_ID.", '".$Soll_Beg_DtmDB."', '".$Soll_End_DtmDB."', NULL, NULL, 0, 102, ".$Projekt_ID.", ".$vMPID.")";
				$err=db_x($query_zeit);
fxDebug($err,$query_zeit, 2);
			}

			// Zusatzprojektverantwortliche festlegen
			if(!$err && $Projekt_ID)
			{
				$pvarr=array();
				if(fxIsArray($_POST))
				{
					foreach($_POST as $feldname => $feldwert)
					{
						if(substr($feldname, 0, 4) == 'pvr_')
							$pvarr[(int)substr($feldname, 4)]=(int)$feldwert;
					}
				}
				if(fxIsArray($pvarr))
					$err=update_zusatzpv($Projekt_ID, $pvarr, true);
			}

			// Vorgangsnummer auf Eindeutigkeit prüfen
			if(!$err)
				$err=check_vn($Projekt_ID, false);

			// Evtl. Status übergeordneter Projekte anpassen
			if(!$err)
				$err=set_status($Projekt_ID, $Projekt_Status);

			// safe the user form fields, according to the current action
			if(!$err)
				$err=($err || safe_user_form_fields($dbwerte, $GLOBALS['fxpglobals']['action'], $Projekt_ID));

			// Erfolgsmeldung vorbereiten
			if(!$err)
		    {
				// Enthält die Eigenschaften die "vererbt" werden von übergeordneten Vorgängen + Abhängigkeit
				if($Deadline)
					$Deadline_txt=$Deadline;
				else
					$Deadline_txt=meldung(31,false);	// leer

				$Properties=get_text(48, '20').' "'.$Deadline_txt.'", '.get_text(69, '20').' "'.meldung($Abrechnungsart).'", '.get_text(2049, '20').' "'.meldung($Abrechnungsart_Mat).'", '.get_text(47, '20').' "'.meldung($Prioritaet).'"';
				$next=2;
			}
			// Fehlermelung
			else
			{
				error_msg(458, false);
				$next=false;
			}
		break;

		case FXP_CHANGE:
			if(!fxIsArray($history))
				$err=true;

			$archived='NULL';
			if($vPART == FXP_PRJ_MAIN)
			{
				$vMPID=$Projekt_ID;
				$vEPID=0;

				if($_POST['projekt_archiviert'] && ($Projekt_Status == FXP_PS_COMPLETED))
					$archived='1';
			}
			else if($vEPID)
				$vMPID=db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			else if($Projekt_ID)
				$vMPID=db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			else
				$vMPID=0;

			// Falls der Kunde einer Aufgabe geändert wurde, müssen diese Änderungen auch bei den Zeitdaten durchgeführt werden (falls bereits abgerechnet ist Auswahlfeld disabled, siehe prj_c_daten_pre.inc)
			if(!$err && ($vPART == FXP_PRJ_TASK) && ($history['kunde'] != $Kunde))
			{
				$sql="UPDATE zeitdaten SET zkunde=".$Kunde." WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.")";
				$err=db_x($sql);
fxDebug($err, $sql, 2);
			}

			// Hat sich der Elter geändert, d.h. soll der Vorgang im Projektbaum verschoben werden?	
			// Bei einer Projektzweigverschiebung werden alle Mutterprojekt-IDs der im
			// Projektzweig liegenden Projekte upgedated
			// Wenn der User versucht hat, das Projekt zu verschieben, es aber dann doch nicht
			// gemacht hat (-> OK Klick statt Anpassen) --> ursprüngl. Zeitrahmen und ursprüngl. Position im Baum
			// Achtung: Dies funktioniert nur wenn die Warnmeldung erscheint, ansonsten wird die
			// Verschiebung nämlich sofort durchgeführt (bei 1. OK-Klick), und der User hat 
			// erst garnicht die Möglichkeit sich zwischen "Anpassen" und "OK" zu entscheiden
			if(($stretch_s || $stretch_e) && $Button_okay)
			{
				$Soll_Beg_DtmDB=$history['soll_beginn'];
				$Soll_End_DtmDB=$history['soll_ende'];
				$vEPID=$history['elter'];
				$vMPID=$history['mutterprojekt_id'];
			}

			if($vEPID != $history['elter'])
			{
				if(!$err && $infvn)
				{
					$sql="UPDATE projekte SET mutterprojekt_id=".$vMPID." WHERE projekt_id IN (".$infvn.")";
					$err=db_x($sql);
fxDebug($err, $sql, 2);
					if(!$err)
					{
						// Die Zeitkonto-Einträge werden durch die Verschieben-Funktion verschoben, hier nicht nötig, nur Projekt-Zeiten
						$sql="UPDATE zeitdaten SET mutterprojekt_id=".$vMPID." WHERE projekt_id IN (".$inf_vn.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
						$err=db_x($sql);
fxDebug($err, $sql, 2);
					}
				}

				// Aktualisierung des Aufwand-Soll beim alten Mutterprojekt
				if(!$err && ($history['aufwand_soll'] != 0))
					$err=Verschiebung($history['elter'], $history['aufwand_soll'] * (-1));

				// Aktualisierung beim neuen Mutterprojekt, nur nicht wenn man die Projektart
				// auf "Projekt" ändert, da das akt. Projekt ja dann das Mutterprojekt ist
				if(!$err && $vEPID && ($vPART != FXP_PRJ_MAIN) && ($Aufwand_Soll_up != 0))
					$err=Verschiebung($vEPID, $Aufwand_Soll_up);

				// Eine Aufwandkorrektur ist hier dann nicht mehr nötig
				$correction=false;

				// Aktualisierung des Aufwand-Ist beim alten Mutterprojekt
				if(!$err && ($Aufwand_Ist_up != 0))
				{
					$err=Zeiterfassung($history['elter'], -$Aufwand_Ist_up);

					// Aktualisierung beim neuen Mutterprojekt, nur nicht wenn man die Projektart
					// auf "Projekt" ändert, da das akt. Projekt ja dann das Mutterprojekt ist
					if(!$err && $vEPID && ($vPART != FXP_PRJ_MAIN) && ($Aufwand_Ist_up != 0))
						$err=Zeiterfassung($vEPID, $Aufwand_Ist_up);
				}

				// Änderung von Aufgabe in (Teil-)Projekt --> Soll-Aufwand=0 (Ist-Aufwand muss eh 0 sein, da sonst nicht erlaubt!)
				if((($vPART == FXP_PRJ_MAIN) || ($vPART == FXP_PRJ_SUB)) && ($ProjektART == FXP_PRJ_TASK))
					$Aufwand_Soll_up=0;
				if(($vPART <= FXP_PRJ_SUB) && ($history['projektart'] == FXP_PRJ_TASK))
					$Aufwand_Soll_up=0;
			}

			if(!$err && $history['vorgangsnummer'] && ($Vorgangsnummer_new != $history['vorgangsnummer']))
			{
				// Existiert die VN bereits
				$vnexists=exist_vn($Projekt_ID, $Vorgangsnummer_new);
				// und wurde der Vorgang nach "hinten" verschoben (außer bei Mutterprojekten oder bei Verschiebung im Baum)
				if($vnexists && ((int)substr($Vorgangsnummer_new, -4) > (int)substr($history['vorgangsnummer'], -4)) && $vEPID && ($vEPID == $history['elter']))
					$Vorgangsnummer_new .= 'a';	// <-- Qualifier der dafür sorgt, dass der Vorgang "hinter" den mit der gleichen VN gesetzt wird
			}

			$query_proj  = "UPDATE projekte SET mutterprojekt_id=".$vMPID.", elter=".$vEPID.", name_projekt=".$Name_Projekt.", projektart=".$vPART.", projektleiter=".$Projektleiter.",";
			$query_proj .= " projektmanager=".$Projektmanager.", vertreter=".$Vertreter.", ansprechpartner=".$Ansprechpartner.", kunde=".$Kunde.", projekt_status=".$Projekt_Status.",";
			$query_proj .= " prioritaet=".$Prioritaet.", beschreibung=".$Beschreibung.", aufwand_soll=".$Aufwand_Soll_up.", max_zeit_aufw=".$Max_Zeit_Aufw_up.",";
			$query_proj .= " anmerkung=".$Anmerkung_up.", anmerkungintern=".$AnmerkungIntern_up.", deadline=".$Deadline_up.", maart_id=".$MaArt_ID.", mafaehigkeit_id=".$MaFaehigkeit_ID.",";
			$query_proj .= " kategorie_id=".$Kategorie_ID.", projekt_archiviert=".$archived.", vorgangsnummer='".$Vorgangsnummer_new."', ref_nummer=".$refnr.$loc_colval3.$app_colval3;
			$query_proj .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND projekt_id=".$Projekt_ID;
			if(!$err)
			{
				$err=db_x($query_proj);
fxDebug($err,$query_proj, 2);
			}

			$query_zeit  = "UPDATE zeitdaten SET soll_beginn='".$Soll_Beg_DtmDB."', soll_ende='".$Soll_End_DtmDB."', zeitart=102, projekt_id=".$Projekt_ID.", mutterprojekt_id=".$vMPID;
			$query_zeit .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart=102 AND projekt_id=".$Projekt_ID;
			if(!$err)
			{
				$err=db_x($query_zeit);
fxDebug($err,$query_zeit, 2);
			}

			// Aktualisieren des Verzeichnisses
			if(!$err && $history['vorgangsnummer'] && ($Vorgangsnummer_new != $history['vorgangsnummer']) && $vMPID)
			{
				$err=verzeichnis($vMPID);
				// Prüfen, ob Vorgangsnummern endeutig
				if(!$err)
					$err=check_vn($vMPID, true);
			}

			// Wurde die Projektkategorie, der Ansprechpartner, der stellv. Projektleiter, der Projektleiter oder der Projektmanager verändert?
			if(!$err && $infvn && $sub_change)
			{
				$ca=array
				(
					'ansprechpartner'	=> array('mode'=>(($sub_change >> 0) & 3),	'old'=>$history['ansprechpartner'],	'new'=>$Ansprechpartner),
					'kunde'				=> array('mode'=>(($sub_change >> 0) & 3),	'old'=>$history['kunde'],			'new'=>$Kunde),
					'vertreter'			=> array('mode'=>(($sub_change >> 2) & 3),	'old'=>$history['vertreter'],		'new'=>$Vertreter),
					'projektleiter'		=> array('mode'=>(($sub_change >> 4) & 3),	'old'=>$history['projektleiter'], 	'new'=>$Projektleiter),
					'projektmanager'	=> array('mode'=>(($sub_change >> 6) & 3),	'old'=>$history['projektmanager'],	'new'=>$Projektmanager),
					'kategorie_id'		=> array('mode'=>(($sub_change >> 8) & 3),	'old'=>$history['kategorie_id'],	'new'=>$Kategorie_ID)
				);
fxDebug($ca, '$ca: $sub_change='.$sub_change, 3);

				$err=sub_change($Projekt_ID, $ca);
				// 2525: Fehler beim Anpassen der [Hauptverantwortlichen Personen] in den untergeordneten Vorgängen.
				// 2777: Fehler beim Anpassen der [Projektkategorie] in den untergeordneten Vorgängen.
				if($err)
					error_msg($err);
			}

			// Wird gesetzt wenn Verschiebung von abhängigen Projekten gefordert wird
			if(!$err && $update && fxIsArray($call_for_DB))
			{
				foreach($call_for_DB as $db => $call)
				{
					if(!$call)
						continue;
					$err=db_x($call);
fxDebug($err, $call, 2);
					if($err)
						break;
				}
			}

			// Zusatzprojektverantwortliche ändern
			if(!$err && $Projekt_ID)
			{
				$pvarr=array();
				if(fxIsArray($_POST))
				{
					foreach($_POST as $feldname => $feldwert)
					{
						if(substr($feldname, 0, 4) == 'pvr_')
							$pvarr[(int)substr($feldname, 4)]=(int)$feldwert;
					}
				}
				if(sizeof($pvarr))
					$err=update_zusatzpv($Projekt_ID, $pvarr, true);
			}

			// Status ändern (falls Vorgang abgeschlossen wird, dann werden die Ist-Endzeiten autom. eingetragen)
			if(!$err && $Projekt_ID && fxIsArray($history) && ($history['projekt_status'] != $Projekt_Status))
				$err=set_status($Projekt_ID, $Projekt_Status);

			// Bei Änderungen Maplanung anpassen
			if(!$err)
			{
				$err=update_maplanung('person', $GLOBALS['fxpglobals']['client'], $Projekt_ID, '', '', false, true, false, false);
				if(!$err)
					$err=update_maplanung('sachmittel', $GLOBALS['fxpglobals']['client'], $Projekt_ID, '', '', false, true, false, false);
			}

			// safe the user form fields, according to the current action
			if(!$err)
				$err=safe_user_form_fields($dbwerte, $GLOBALS['fxpglobals']['action'], $Projekt_ID);

			// Fehlermeldung
			if($err)
			{
				if($err === true)
		       		error_msg(460, false);	// Fehler beim Ändern des Datensatzes.
		       	else
		       		error_msg($err, false);
		    }
		break;

		case FXP_DELETE:
			if($vPART == FXP_PRJ_MAIN)
			{
				$vMPID=$Projekt_ID;
				$vEPID=0;
			}
			else if($vEPID)
				$vMPID=db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			else if($Projekt_ID)
				$vMPID=db_value("SELECT mutterprojekt_id FROM projekte WHERE projekt_id=".$Projekt_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			else
				$vMPID=0;

			// Get all project relevant documents
			$da=delProjectDocsArray($Projekt_ID,$da,false);
fxDebug($da,'$da', 3);

			// Get all project relevant tables in which data needs to be deleted
			$pta=delProjectArray($Projekt_ID);
			if(fxIsArray($pta))
			{
				$pn='<b>'.no_null($history['vorgangsnummer']).'</b>: '.$history['name_projekt'];
				foreach($pta as $ptable => $sql)
				{
					// Don't delete budget tables "budget_summe" and "budget_detail", because they are needed for calculations in function "update_budget", which will then delete them
					if(substr($ptable,0,7) != 'budget_')
					{
						$err=db_x($sql);
fxDebug($err, $sql, 2);
						if($err)
						{
							// 2983: $var1Error deleting project $var2$var3
							error_msg(2983,false,'','&quot;'.$pn.'&quot;','!<br /><i class=grey>['.$ptable.']</i>');
							break;
						}
					}
				}
			}
	   	break;
	}

	// wenn der Aufwand geändert wurde, oder bei Neuanlage oder Löschen eines Vorgangs, müssen die Daten der übergeordneten Vorgänge dementsprechend korrigiert werden
	if(!$err && $correction && ($gap_aufwand != 0))
	{
		// Ermittelt alle übergeordneten Vorgänge
		$above_p='';
		if(($GLOBALS['fxpglobals']['action'] != FXP_DELETE) && $Projekt_ID)
			$above_p=superior_vn($Projekt_ID, false);
		// Beim Löschen ist hier der Vorgang bereits gelöscht
		else if($vEPID)
			$above_p=superior_vn($vEPID, true);

		if($above_p && ($above_p != '-666'))
		{
			$query_p="UPDATE projekte SET aufwand_soll=(aufwand_soll + (".$gap_aufwand.")) WHERE projekt_id IN (".$above_p.") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$err=db_x($query_p);
fxDebug($err,$query_p, 2);
		}
	}

	// Elter-Zeitrahmen anpassen
	if(!$err && $_POST['Button_Anpassen'] && $vEPID)
	{
		$upper=superior_vn($vEPID, true);
		if(!$err && ($stretch_s || $stretch_OKs))
		{
			$stretch_sql="UPDATE zeitdaten SET soll_beginn='".$Soll_Beg_DtmDB."' WHERE zeitart=102 AND projekt_id IN (".$upper.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND soll_beginn>'".$Soll_Beg_DtmDB."'";
			$err=db_x($stretch_sql);
fxDebug($err,$stretch_sql, 2);
		}
		if(!$err && ($stretch_e || $stretch_OKe))
		{
			$stretch_sql="UPDATE zeitdaten SET soll_ende='".$Soll_End_DtmDB."' WHERE zeitart=102 AND projekt_id IN (".$upper.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND soll_ende<'".$Soll_End_DtmDB."'";
			$err=db_x($stretch_sql);
fxDebug($err,$stretch_sql, 2);
		}
	}

	// Budgetdaten verwalten
	if(!$err && $Projekt_ID)
	{
		// Daten aufbereiten
		$barr=array
		(
			'budgettyp'				=> $budgettyp,
			'abrechnungsart'		=> $Abrechnungsart,				'abrechnungsart_mat'	=> $Abrechnungsart_Mat,
			'budgetrahmen_pers_int'	=> $budgetrahmen_pers_int_up,	'budgetrahmen_mat_int'	=> $budgetrahmen_mat_int_up,
			'budgetrahmen_pers_ext'	=> $budgetrahmen_pers_ext_up,	'budgetrahmen_mat_ext'	=> $budgetrahmen_mat_ext_up,
			'budget_pers_int_gepl'	=> $budget_pers_int_gepl_up,	'budget_mat_int_gepl'	=> $budget_mat_int_gepl_up,
			'budget_pers_ext_gepl'	=> $budget_pers_ext_gepl_up,	'budget_mat_ext_gepl'	=> $budget_mat_ext_gepl_up
		);
		$err_id=update_budget($Projekt_ID, $barr);
		if($err_id)
		{
			error_msg($err_id, false);
			$err=true;
		}
	}

	// PCA-Code verwalten
	if(!$err && $Projekt_ID)
	{
//($pid, $def='', $ret_error=false)
		$err=pca_code($Projekt_ID, '', true);
		if($err)
			error_msg($err, false);
	}

	// Prefix (nur beim Mutterprojekt)
	if(!$err && $vMPID && ($vPART == FXP_PRJ_MAIN))
	{
		$ppn=trim($_POST['pprefix']);
		$err=handle_prefix($GLOBALS['fxpglobals']['action'], $vMPID, $Kunde, $ppn, true);
		if($err)
		{
			// 2312: Fehler beim Verwalten des Vorgangsnamen-Prefixes $var1für Projekt $var2!
			if(strlen($ppn))
				$ppn='"<b>'.$ppn.'</b>" ';
			error_msg(2312, false, $ppn, '<b>'.no_null($Vorgangsnummer_new).'</b>');
		}
	}

	// Wiederholungen
	$Anzahl=(int)$Anzahl;
	if(!$err && ($Anzahl > 1))
	{
		$datereset=false;
		$minus=false;
		$check_DTM=$Soll_Beg_DtmDB;
		$thirties=array(3,5,8,10);
		$quarterly=array(1,3,6,8,11);

		$max_soll_end_dtm='99999999240000';
		if(!$_POST['Button_Anpassen'] && $vEPID)
		{
			$sql="SELECT soll_ende FROM zeitdaten WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart=102";
			$max_soll_end_dtm=substr(db_value($sql), 0, 8).'240000';
		}
fxDebug($max_soll_end_dtm, '$max_soll_end_dtm', 3);

		$Soll_Beg_DtmDB_bak=$Soll_Beg_DtmDB;
		for($c=0; $c<($Anzahl-1); $c++)
		{
			switch($Wiederholungart)
			{
				case 665:	// Täglich
					$Soll_Beg_DtmDB=time_add($Soll_Beg_DtmDB, 1, 'D');
				break;

				case 666:	// Wöchentlich
					$Soll_Beg_DtmDB=time_add($Soll_Beg_DtmDB, 7, 'D');
				break;

				case 667:	// Monatlich
					// wurde das Datum auf den letzten Kalendertag des Monats zurückgesetzt, dann wird für die nächste Berechnung wieder der eigentlich Endtag benutzt
					if($datereset)
					{
						$Soll_Beg_DtmDB=substr($Soll_Beg_DtmDB, 0, 6).substr($check_DTM, 6, 2).'000000';
						$datereset=false;
					}
					// würde der letzte Kalendertag des Monats überschritten werden, dann auf den letzten Kalendertag setzen
					if((((int)substr($check_DTM, 6, 2) > 28) && ((int)substr($Soll_Beg_DtmDB, 4, 2) == 1)) || (((int)substr($check_DTM, 6, 2) > 30) && in_array((int)substr($Soll_Beg_DtmDB, 4, 2), $thirties)))
					{
						$Soll_Beg_DtmDB=substr(ultimo(time_add($Soll_Beg_DtmDB,7,'D')), 0,8).'000000';
						$datereset=true;
					}
					else
						$Soll_Beg_DtmDB=time_add($Soll_Beg_DtmDB, 1, 'M');
				break;

				case 664:	// Vierteljährlich
					// wurde das Datum auf den letzten Kalendertag des Monats zurückgesetzt, dann wird für die nächste Berechnung wieder der eigentlich Endtag benutzt
					if($datereset)
					{
						$Soll_Beg_DtmDB=substr($Soll_Beg_DtmDB, 0, 6).substr($check_DTM, 6, 2).'000000';
						$datereset=false;
					}
					// würde der letzte Kalendertag des Monats überschritten werden, dann auf den letzten Kalendertag setzen
					if((((int)substr($check_DTM, 6, 2) > 28) && ((int)substr($Soll_Beg_DtmDB, 4, 2) == 11)) || (((int)substr($check_DTM, 6, 2) > 30) && in_array((int)substr($Soll_Beg_DtmDB, 4, 2), $quarterly)))
					{
						$Soll_Beg_DtmDB=substr(ultimo(time_add($Soll_Beg_DtmDB,7,'D')), 0,8).'000000';
						$datereset=true;
					}
					else
						$Soll_Beg_DtmDB=time_add($Soll_Beg_DtmDB, 3, 'M');
				break;

				case 668:	// Jährlich
					$Soll_Beg_DtmDB=(substr($Soll_Beg_DtmDB, 0, 4) + 1).substr($Soll_Beg_DtmDB, 4);
					// 29. Februar
					if(substr($check_DTM, 4, 4) == '0229')
					{
						if((fx_date('L', $tstamp) == 0) && !$minus)
						{
							$Soll_Beg_DtmDB=time_add($Soll_Beg_DtmDB, -1, 'D');
							$minus=true;
						}
						else if(fx_date('L', $tstamp) == 1)
						{
							$Soll_Beg_DtmDB=time_add($Soll_Beg_DtmDB, +1, 'D');
							$minus=false;
						}
					}
				break;

				default;	// Gleiches Datum
					$Soll_Beg_DtmDB=$Soll_Beg_DtmDB_bak;
				break;
			}
			if(isset($GLOBALS['_cppj']))
				unset($GLOBALS['_cppj']);
			$tad=time_diff($Soll_Beg_DtmDB_bak, $Soll_Beg_DtmDB, 'D');
			$err=copy_project($Projekt_ID, $vEPID, 1, 0, $tad, 2, $GLOBALS['fxpglobals']['client'], false, true);
//echo('$err=copy_project($src_id='.$Projekt_ID.', $dst_id='.$vEPID.', $inf=1, $res=0, $tad='.$tad.', $timeadjust=2, $mand='.$GLOBALS['fxpglobals']['client'].', $addinfo=false, $dbaccess=true);<BR>-&gt; '.$err.'<BR>');
			if($err)
			{
				error_msg($err);
				break;
			}

			if($vEPID)
			{
				// Prüfen, ob Zeitrahmen überschritten
				$sql="SELECT soll_ende FROM zeitdaten WHERE projekt_id=".$vEPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND zeitart=102";
				$akt_soll_end_dtm=substr(db_value($sql), 0, 8).'240000';
//fxDebug($akt_soll_end_dtm, '$akt_soll_end_dtm');
				if($akt_soll_end_dtm > $max_soll_end_dtm)
				{
					// 1139: Der Endtermin ($var1) eines Wiederholungsvorgangs liegt nach dem Endtermin ($var2) des übergordneten Vorgangs!
					error_msg(1139, false, get_entry($akt_soll_end_dtm, 'datum'), get_entry($max_soll_end_dtm, 'datum'));
					$err=true;
				}

				// Prüfen, ob Budgetrahmen überschritten
				if($vPART == FXP_PRJ_TASK)
				{
					$sql  = "SELECT budgetrahmen_pers_int, budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext,";
					$sql .= " budget_pers_int_gepl, budget_pers_ext_gepl, budget_mat_int_gepl, budget_mat_ext_gepl";
					$sql .= " FROM budget_summe WHERE projekt_id=".$vEPID;
					$bdt=db_values($sql);
				}
				else
				{
					$sql  = "SELECT budgetrahmen_pers_int, budgetrahmen_pers_ext, budgetrahmen_mat_int, budgetrahmen_mat_ext";
					$sql .= " FROM budget_summe WHERE projekt_id=".$vEPID;
					$tmp=db_values($sql);
					if(!fxIsArray($tmp))
						$tmp=array('budgetrahmen_pers_int'=>0.00, 'budgetrahmen_pers_ext'=>0.00, 'budgetrahmen_mat_int'=>0.00, 'budgetrahmen_mat_ext'=>0.00);
					$sql ="SELECT ".$tmp['budgetrahmen_pers_int']." AS budgetrahmen_pers_int, ".$tmp['budgetrahmen_pers_ext']." AS budgetrahmen_pers_ext,";
					$sql .= " ".$tmp['budgetrahmen_mat_int']." AS budgetrahmen_mat_int, ".$tmp['budgetrahmen_mat_ext']." AS budgetrahmen_mat_ext,";
					$sql .= " SUM(b.budgetrahmen_pers_int) AS budget_pers_int_gepl, SUM(b.budgetrahmen_pers_ext) AS budget_pers_ext_gepl,";
					$sql .= " SUM(b.budgetrahmen_mat_int) AS budget_mat_int_gepl, SUM(b.budgetrahmen_mat_ext) AS budget_mat_ext_gepl";
					$sql .= " FROM budget_summe b, projekte p WHERE b.projekt_id=p.projekt_id AND p.elter=".$vEPID;
					$bdt=db_values($sql);
				}
fxDebug($bdt, $sql, 3);
				if(is_array($bdt))
				{
					foreach($iea as $ie)
					{
						if($bdt['budgetrahmen_pers_'.$ie] && ($bdt['budget_pers_'.$ie.'_gepl'] > $bdt['budgetrahmen_pers_'.$ie]))
						{
							// 1140: Der $var1 Personenbudgetrahmen wurde durch einen Wiederholungsvorgang um $var2 überschritten! Ihnen stehen nur maximal $var3 zur Verfügung - Sie haben aber $var4 geplant. Bitte verkleinern Sie den neuen Planwert oder erhöhen Sie den Budgetrahmen.
							$diff=$bdt['budget_pers_'.$ie.'_gepl']-$bdt['budgetrahmen_pers_'.$ie];
							error_msg(1140, false, $ie.'.', get_entry($diff, 'betrag').$mwaehrung, get_entry($bdt['budgetrahmen_pers_'.$ie], 'betrag').$mwaehrung, get_entry($bdt['budget_pers_'.$ie.'_gepl'], 'betrag').$mwaehrung);
							$err=true;
						}
						if($bdt['budgetrahmen_mat_'.$ie] && ($bdt['budget_mat_'.$ie.'_gepl'] > $bdt['budgetrahmen_mat_'.$ie]))
						{
							// 1153: Der $var1 Materialbudgetrahmen wurde durch einen Wiederholungsvorgang um $var2 überschritten! Ihnen stehen nur maximal $var3 zur Verfügung - Sie haben aber $var4 geplant. Bitte verkleinern Sie den neuen Planwert oder erhöhen Sie den Budgetrahmen.
							$diff=$bdt['budget_mat_'.$ie.'_gepl']-$bdt['budgetrahmen_mat_'.$ie];
							error_msg(1153, false, $ie.'.', get_entry($diff, 'betrag').$mwaehrung, get_entry($bdt['budgetrahmen_mat_'.$ie], 'betrag').$mwaehrung, get_entry($bdt['budget_mat_'.$ie.'_gepl'], 'betrag').$mwaehrung);
							$err=true;
						}
					}
				}

				if($err)
					break;
			}
		}
	}

	// Ressourcenplanung
	if(!$err && $Projekt_ID && ($vPART == FXP_PRJ_TASK) && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
	{
		$r=new RESOURCE(false,true,false,false,true);
		if($r)
		{
			if(!isset($_POST['Personen_ID']))
				$r->setShowMessages(false);

			$r->getResources($Projekt_ID);

			if(isset($_POST['Personen_ID']))
			{
				// Alte Ressource löschen
				$r->delResources($Projekt_ID);
				// Neue Ressource zuordnen, dabei berücksichtigen ob EK/VK-Felder überhaupt angezeigt werden
				$psid=(int)$_POST['Personen_ID'];
				if($psid)
				{
					$errv=false;
					$budgetart=strtoupper(substr($GLOBALS['fxpglobals']['settings']['budgetart'], 0, 1)); // [N]=Keine Budgetart, [I]=Nur int. Budgetart, [E]=Nur ext. Budgetart + [B]=Beide Budgetarten
					if(($budgetart == 'N') || ($budgetart == 'E'))
						$ekb=0.00;
					else
					{
						$ekb='';
						if(strlen(trim($_POST['EK'])))
							$ekb=validate_entry($_POST['EK'], $errv, 'betrag');
					}
					if(($budgetart == 'N') || ($budgetart == 'I'))
						$vkb=0.00;
					else
					{
						$vkb='';
						if(strlen(trim($_POST['VK'])))
							$vkb=validate_entry($_POST['VK'], $errv, 'betrag');
					}
					$r->addResource($Projekt_ID, $psid, '', $ekb, $vkb);
				}
			}

			$err=$r->assignResources();
			unset($r);
		}
		else
			$err=true;
	}

	// Steuervariablen wieder zurück setzen
	$GLOBALS['fxptdata']['backup']['Backup']['chk_we_start']=false;
	$GLOBALS['fxptdata']['backup']['Backup']['chk_we_end']=false;

	$stretch_s=false;
	$stretch_e=false;
	$update=false;

	protectVar($stretch_s, 'stretch_s');
	protectVar($stretch_e, 'stretch_e');
	protectVar($update, 'update');

	// Wurde die Deadline geändert oder wurde ein Vorgang gelöscht
	if(!$err && $Projekt_ID && (($before_change['deadline'] && (strtodatetime($GLOBALS['fxpglobals']['settings']['formatdatum'], $Deadline) != $before_change['deadline'])) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
	{
		$err=set_deadline($Projekt_ID);
	}

	// Meilensteintermine anpassen, falls Start- oder Endtermin verändert wurde
	if(!$err && $Projekt_ID && ($GLOBALS['fxpglobals']['action'] != FXP_DELETE) && (!fxIsArray($history) || ($Soll_Beg_DtmDB != $history['soll_beginn']) || ($Soll_End_DtmDB != $history['soll_ende'])))
	{
		$err=setMilestoneDates($Projekt_ID);
		if($err)
		{
			// 2864: Fehler beim Anpassen der Meilensteintermine!
			error_msg(2864, false);
		}
	}
//$err=true;

	// Delete documents
	if(!$err && sizeof($da))
		delProjectDocsArray(false,$da);

	// Handle all project eMails
	$msent=false;
	if(!$err)
	{
		$msent=sendProjectMails();
		if(strlen($GLOBALS['_sent_success']) || strlen($GLOBALS['_sent_error']))
		{
			if(strlen($GLOBALS['_sent_error']))
			{
				$ec=substr_count($GLOBALS['_sent_error'], '<br />');
				if($ec == 1)
					error_msg(3069,false,':'.$GLOBALS['_sent_error']);			// 3069: The following person could not be informed by Email$var1
				else
					error_msg(3070,false,$ec,':'.$GLOBALS['_sent_error']);		// 3070: The following $var1 persons could not be informed by Email$var2
			}
			if(strlen($GLOBALS['_sent_success']))
			{
				$ec=substr_count($GLOBALS['_sent_success'], '<br />');
				if($ec == 1)
					error_msg(3071,false,':'.$GLOBALS['_sent_success']);		// 3071: The following person has been successfully informed by Email$var1
				else
					error_msg(3072,false,$ec,':'.$GLOBALS['_sent_success']);	// 3072: The following $var1 persons have been successfully informed by Email$var2
			}
		}
	}
//$err=true;

	// Am Ende des Updates => bisherigen Stand löschen, bei erneutem Aufruf wird der neue Stand geladen
	if(!$err && isset($history))
		unset($history);

	// Erfolgsmeldungen
	if(!$err)
	{
		// Ressourcen neu ermitteln für Aufgaben
		if($Projekt_ID && ($vPART == FXP_PRJ_TASK) && ($GLOBALS['fxpglobals']['action'] != FXP_CREATE))
		{
			$res=prj_get_resources($Projekt_ID);
			$sor=sizeof($res);
		}
//fxDebug($res, __FILE__.' ['.__LINE__.'] -- $res: ('.$sor.')');

		// Wurde ein Vorgang neu angelegt oder gelöscht, dann evtl. Filter anpassen
		if((($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE)))
		{
			$new_filter_settings=array();

			// Neuen Filter ermitteln...
			if(strlen($GLOBALS['fxpglobals']['filter']['projektname']))
				$new_filter_settings['projektname']='';
			if($GLOBALS['fxpglobals']['filter']['proj_menu_von'] && ($GLOBALS['fxpglobals']['filter']['proj_menu_von'] > $Soll_Beg_DtmDB))
				$new_filter_settings['proj_menu_von']=$Soll_Beg_DtmDB;
			if($GLOBALS['fxpglobals']['filter']['proj_menu_bis'] && ($GLOBALS['fxpglobals']['filter']['proj_menu_bis'] < $Soll_End_DtmDB))
				$new_filter_settings['proj_menu_bis']=$Soll_End_DtmDB;
			if($GLOBALS['fxpglobals']['filter']['mutterprojekt_id'][0])
				$new_filter_settings['mutterprojekt_id']=array($vMPID);
			if($GLOBALS['fxpglobals']['filter']['kunde'][0])
				$new_filter_settings['kunde']=array($Kunde);
			if($GLOBALS['fxpglobals']['filter']['projektmanager'][0])
			{
				if($Projektmanager)
					$new_filter_settings['projektmanager']=array($Projektmanager);
				else
					$new_filter_settings['projektmanager']=array(0);
			}
			if($GLOBALS['fxpglobals']['filter']['mafaehigkeit_id'][0])
			{
				if($MaFaehigkeit_ID)
					$new_filter_settings['mafaehigkeit_id']=array($MaFaehigkeit_ID.'f');
				else if($MaArt_ID)
					$new_filter_settings['mafaehigkeit_id']=array($MaArt_ID);
				else
					$new_filter_settings['mafaehigkeit_id']=array(0);
			}
			if($GLOBALS['fxpglobals']['filter']['projektleiter'][0])
			{
				$new_filter_settings['projektleiter']=array($Projektleiter);
				if($Vertreter)
					$new_filter_settings['projektleiter'][]=$Vertreter;
			}
			if($GLOBALS['fxpglobals']['filter']['kategorie_id'][0])
			{
				if($Kategorie_ID)
					$new_filter_settings['kategorie_id']=array($Kategorie_ID);
				else
					$new_filter_settings['kategorie_id']=array(0);
			}
			if(!$GLOBALS['fxpglobals']['filter']['projektstatus_'.$Projekt_Status])
				$new_filter_settings['projektstatus_'.$Projekt_Status]=1;
			if(!$GLOBALS['fxpglobals']['filter']['prioritaet_'.$Prioritaet])
				$new_filter_settings['prioritaet_'.$Prioritaet]=1;
			if($GLOBALS['fxpglobals']['filter']['nurauftrag'])
				$new_filter_settings['nurauftrag']='';

			// ...und Filter setzen...
			if(sizeof($new_filter_settings))
			{
				$store_filter_settings=false;
				foreach($new_filter_settings as $flt_var => $flt_val)
				{
					if(!is_array($flt_val) && ($GLOBALS['fxpglobals']['filter'][$flt_var] != $flt_val))
					{
						$GLOBALS['fxpglobals']['filter'][$flt_var]=$flt_val;
//echo('NEW Filter-Setting: '.$flt_var.' -> '.$GLOBALS['fxpglobals']['filter'][$flt_var].'<br>');
						$store_filter_settings=true;
					}
					else if(is_array($flt_val))
					{
						if(!$flt_val[0])
						{
							$GLOBALS['fxpglobals']['filter'][$flt_var]=array(0);
//echo('NEW Filter-Setting: '.$flt_var.' -> array(0)<br>');
							$store_filter_settings=true;
						}
						else
						{
							foreach($flt_val as $flt_new_val)
							{
								$val_found=false;
								foreach($GLOBALS['fxpglobals']['filter'][$flt_var] as $flt_orig_val)
								{
									if($flt_new_val == $flt_orig_val)
									{
										$val_found=true;
										break;
									}
								}
								if(!$val_found)
								{
									$GLOBALS['fxpglobals']['filter'][$flt_var][]=$flt_new_val;
//echo('NEW Filter-Setting: '.$flt_var.'[] -> '.$flt_new_val.'<br>');
									$store_filter_settings=true;
								}
							}
						}
					}
				}

				// ...und in die Datenbank schreiben
				if($store_filter_settings)
				{
					$ferr=store_settings(filter_string(false), 223);
					if($ferr)
						error_msg(71, false);	// 71: Die Filtereinstellungen wurden gesetzt jedoch konnten die Werte nicht für eine spätere Session in die Datenbank übernommen werden!
					else
						error_msg(70, false);	// 70: Die Filtereinstellungen wurden erfolgreich gesetzt.
				}
			}
		}

		// Wurde ein Vorgang neu angelegt oder gelöscht, dann Masken ausblenden
		if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
		{
			// set maske{$maske_id}_nv for all forms but form 222 and 317
			extract(hide_forms($tr, array(222, 317)));

			// Struktur logisch vorbelegen
			if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
			{
				if($vPART == FXP_PRJ_MAIN)
				{
					$vPART=FXP_PRJ_SUB;
					$vEPID=$Projekt_ID;
				}
				else if($vPART == FXP_PRJ_SUB)
				{
					$vPART=FXP_PRJ_TASK;
					$vEPID=$Projekt_ID;
				}
			}
		}

		if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
			error_msg(2251, false, $Properties);	// Neuanlage erfolgreich
		else if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
			error_msg(459, false);					// Ändern erfolgreich
		else
			error_msg(461, false);					// Löschen erfolgreich
	}

	// Wurde ein Vorgang verändert, dann Info neu laden
	if(!$err && $Projekt_ID && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		$pinfo=projektinfo($Projekt_ID, false, false, true);
}
?>