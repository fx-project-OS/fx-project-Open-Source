<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_f_info_val.inc                                           //
// Version     : 21.1                                                         //
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
 * @version 21.1
 */

if(fxIsArray($_POST) && isset($_POST['Aufwand_Soll']) && isset($_POST['Button_okay']))
{
	$call_for_DB=array();

	// Aufbereiten des Datums in das Datenbankformat (ZWINGEND ERFORDERLICH!)
	if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
		$Erstelldatum=$GLOBALS['datetime'];

	// Ermittlen des Kunden und in bisherige Variable schreiben => d.h. der Rest kann unverändert bleiben (DB-Calls)
	$Kunde=(int)db_value("SELECT elter FROM personen WHERE personen_id=".(int)$Ansprechpartner." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

	// Validierte Werte in Variablen für DB-Call schreiben
	if(substr($maskenfeldwerte['soll_beg_dtm'], 0, 1) == "^")
		$err_sb=true;
	if(substr($maskenfeldwerte['soll_end_dtm'], 0, 1) == "^")
		$err_se=true;
	if(substr($maskenfeldwerte['deadline'], 0, 1) == "^")
		$err_dl=true;

	if(!$Aufwand_Soll)
		$Aufwand_Soll_up=0.00;
	else
		$Aufwand_Soll_up=$maskenfeldwerte['aufwand_soll'];

	if(!$Aufwand_Ist)
		$Aufwand_Ist_up=0.00;
	else
		$Aufwand_Ist_up=$maskenfeldwerte['aufwand_ist'];

	if(!$Deadline)
		$Deadline_up='NULL';
	else
		$Deadline_up="'".substr($maskenfeldwerte['deadline'], 0, 8)."240000'";

	if(!$Max_Zeit_Aufw)
		$Max_Zeit_Aufw_up=0.00;
	else
		$Max_Zeit_Aufw_up=$maskenfeldwerte['max_zeit_aufw'];

	$budarr=array
	(
		'budgetrahmen_pers_int', 'budgetrahmen_mat_int', 'budgetrahmen_pers_ext', 'budgetrahmen_mat_ext',
		'budget_pers_int_gepl', 'budget_mat_int_gepl', 'budget_pers_ext_gepl', 'budget_mat_ext_gepl'
	);
	foreach($budarr as $bdg)
	{
		if(!${$bdg})
			${$bdg.'_up'}=0.00;
		else
			${$bdg.'_up'}=$maskenfeldwerte[$bdg];

		// Prüfung, ob eingegebene Werte positiv sind
		if(${$bdg.'_up'} < 0.00)
		{
			$err=true;
			error_msg(1524, -1, $bdg);	// Bitte geben Sie in das Feld "$var1" eine gültige positive Zahl ein!
		}
	}

	if(!$Anmerkung)
		$Anmerkung_up='NULL';
	else
		$Anmerkung_up="'".convert_string($maskenfeldwerte['anmerkung'], 'todb')."'";

	if(!$AnmerkungIntern)
		$AnmerkungIntern_up='NULL';
	else
		$AnmerkungIntern_up="'".convert_string($maskenfeldwerte['anmerkungintern'], 'todb')."'";

	// Aufwand muss positiv sein
	if($Aufwand_Soll_up < 0.00)
	{
		$err=true;
		error_msg(1524, -1, 'Aufwand_Soll');	// Bitte geben Sie in das Feld "$var1" eine gültige positive Zahl ein!
	}

	// MaArt endet mit 'f' => also eine Fähigkeit gewählt => zugehörige Gruppe bestimmen
	if(substr($MaArt_ID, -1) == 'f')
	{
		$MaFaehigkeit_ID=(int)$MaArt_ID;
		$MaArt_ID=db_value("SELECT maart_id FROM ma_faehigkeit WHERE id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mafaehigkeit_id=".$MaFaehigkeit_ID);
	}
	// Bei Auswahl einer Gruppe bleibt die Fähigkeit auf 0
	else
		$MaFaehigkeit_ID=0;

	// Auswerten der Zeitformate (Drop-Down-Felder)
	$eufa=array('Aufwand_Soll', 'Max_Zeit_Aufw', 'Aufwand_Ist');
	foreach($eufa as $euf)
	{
		$eua=effortUnit2Sec(${$euf.'_up'});
//fxDebug($eua,$euf.': '.${$euf});
		${$euf.'_up'}=$eua['sec'];
	}

	if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
	{
		// Bei Aufgaben tritt ein Fehler auf, wenn das Soll-Ende UND der Aufwand-Soll leer ist
		if(!$Soll_Beg_Dtm && ($vPART == FXP_PRJ_TASK))
		{
			$err=true;
			error_msg(751, -1, 'Soll_Beg_Dtm');	// Das Feld "$var1" darf nicht leer sein. Bitte geben Sie eine Zahl ein!
		}

		// Bei Aufgaben tritt ein Fehler auf, wenn das Soll-Ende UND der Aufwand-Soll leer ist
		if(!$Soll_End_Dtm && !$Aufwand_Soll && ($vPART == FXP_PRJ_TASK))
		{
			$err=true;
			error_msg(751, -1, 'Aufwand_Soll');	// Das Feld "$var1" darf nicht leer sein. Bitte geben Sie eine Zahl ein!
		}

		// Soll-Daten validieren
		// -> beide Felder leer
		if(!$Soll_Beg_Dtm && !$Soll_End_Dtm)
		{
			$Soll_Beg_DtmDB='';
			$Soll_End_DtmDB='';
		}
		// nur Soll-Beginn leer
		else if(!$Soll_Beg_Dtm)
		{
			$err=true;
			error_msg(751, -1, 'Soll_Beg_Dtm');	// Das Feld "$var1" darf nicht leer sein. Bitte geben Sie eine Zahl ein!
		}
		// nur Soll-Ende leer
		else if(!$Soll_End_Dtm && !$err_sb)
		{
			$Soll_Beg_DtmDB=$maskenfeldwerte['soll_beg_dtm'];
			if($Soll_Beg_DtmDB && $Aufwand_Soll)
			{
				$Zeitaufwand_SOLL=$Aufwand_Soll_up;
				$now=$Soll_Beg_DtmDB;
				while($Zeitaufwand_SOLL > 0)
				{
					if(formatdatetime("w", $now) < 6)
					{
						$F_Tag=get_feiertage($now);
						$F_Tag_art=$F_Tag[substr($now, 0, 8)]['art'];
						if(($F_Tag_art != FXP_HL_FULL) && ($F_Tag_art != FXP_HL_HALF))
							$Zeitaufwand_SOLL -= 3600 * $GLOBALS['fxpglobals']['settings']['mtag'];

						if($F_Tag_art == FXP_HL_HALF)
							$Zeitaufwand_SOLL -= 1800 * $GLOBALS['fxpglobals']['settings']['mtag'];
					}
					$now=time_add($now, 1, "D");
					$days++;
				}
			}
			if($days < 1)
				$days=1;
			$now=time_add($Soll_Beg_DtmDB, $days-1, "D");
			$Soll_End_DtmDB=$now;
			$Soll_End_Dtm=get_entry($now, 'datum');
		}
		// beide Felder gefüllt
		else
		{
			// Soll-Daten in DB-Format konvertieren
			$Soll_Beg_DtmDB=$maskenfeldwerte['soll_beg_dtm'];
			$Soll_End_DtmDB=$maskenfeldwerte['soll_end_dtm'];
		}

		if($Soll_Beg_DtmDB)
			$Soll_Beg_DtmDB=substr($Soll_Beg_DtmDB, 0, 8).'000000';
		if($Soll_End_DtmDB)
			$Soll_End_DtmDB=substr($Soll_End_DtmDB, 0, 8).'240000';
	}

	// Bisherige Stand in $history
	if($vAPID && fxIsArray($pinfo) && fxIsArray($pinfo[$vAPID]))
	{
		$history=$pinfo[$vAPID]['data']; 
//fxDebug($history, '$history');
	}
	else
		$history='';

	// Plausibilitätsprüfung der Variablen
	if($db_update && ($GLOBALS['fxpglobals']['action'] != FXP_DISPLAY) && ($GLOBALS['fxpglobals']['action'] != FXP_DELETE))
	{
		if($vAPID && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
		{
			// Projekt-Status vorher höher als geplant und dann wieder zurück auf geplant -> Fehler, nicht möglich
			if(fxIsArray($history) && ($history['projekt_status'] > FXP_PS_PLANNED) && ($Projekt_Status == FXP_PS_PLANNED))
			{
				$err=true;
				error_msg(2195, array('Projekt_Status'), ' ');	// Der Projekt-Status eines einst aktiven Vorgangs kann nicht auf "geplant" zurück gesetzt werden. $var1
			}

			// Prüfen, ob irgendwelche untergeordneten Vorgänge außerhalb des Zeitrahmens des aktuellen Vorgangs sind, wenn dieser geändert wird
			$sql="SELECT z.soll_beginn FROM zeitdaten z, projekte p WHERE p.zeit_id=z.zeit_id AND p.elter=".$vAPID." AND p.projektart<=".FXP_PRJ_TASK." ORDER BY z.soll_beginn ASC";
			$min_beginn=db_value($sql);
			if($min_beginn)
			{
				if(substr($min_beginn, 0, 8) < substr($Soll_Beg_DtmDB, 0, 8))
				{
					// Ein $var1 eines untergeordeten Projektes ist außerhalb des zeitlichen Rahmens des Projektes, den sie momentan bearbeiten. Dieses Projekt muss spätesten am $var2 beginnen.
					error_msg(1149, false, 'Soll_Beg_Dtm', get_entry($min_beginn, "datum"));
					$err=true;
				}
			}
			$sql="SELECT z.soll_ende FROM zeitdaten z, projekte p WHERE p.zeit_id=z.zeit_id AND p.elter=".$vAPID." AND p.projektart<=".FXP_PRJ_TASK." ORDER BY z.soll_ende DESC";
			$max_ende=db_value($sql);
			if($max_ende)
			{
				if(substr($max_ende, 0, 8) > substr($Soll_End_DtmDB, 0, 8))
				{
					// Ein $var1 eines untergeordeten Projektes ist außerhalb des zeitlichen Rahmens des Projektes, den sie momentan bearbeiten. Dieses Projekt darf frühestens am $var2 enden.
					error_msg(2116, false, 'Soll_End_Dtm', get_entry($max_ende, "datum"));
					$err=true;
				}
			}

			// Existieren abgerechnete Zeiten oder Materialien?
			$exist_inv_prs=(int)db_value("SELECT zeit_id FROM zeitdaten WHERE rechnungs_id IS NOT NULL AND rechnungs_id>0 AND projekt_id=".$vAPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
			$exist_inv_mat=(int)db_value("SELECT id FROM materialverbrauch WHERE rechnungs_id IS NOT NULL AND rechnungs_id>0 AND projekt_id=".$vAPID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);

			// 2261: Der Auftraggeber kann nicht mehr verändert werden, da für diese Aufgabe bereits Zeiten oder Materialien abgerechnet wurden!
			if(($exist_inv_prs || $exist_inv_mat) && fxIsArray($history) && ($Kunde != $history['kunde']))
			{
				$err=true;
				error_msg(2261,false);
			}

			// 3163: Die Personenabrechnungsart kann nicht mehr verändert werden, da für diese Aufgabe bereits Zeiten abgerechnet wurden!
			if($exist_inv_prs && fxIsArray($history) && ($Abrechnungsart != $history['abrechnungsart']))
			{
				$err=true;
				error_msg(3163,false);
			}

			// 3164: Die Materialabrechnungsart kann nicht mehr verändert werden, da für diese Aufgabe bereits Materialien abgerechnet wurden!
			if($exist_inv_mat && fxIsArray($history) && ($Abrechnungsart_Mat != $history['abrechnungsart_mat']))
			{
				$err=true;
				error_msg(3164,false);
			}

			if((!$Anzahl && ($Wiederholungart != 0)) || ($Anzahl && ($Wiederholungart == 0)))
			{
				$err=true;
				error_msg(1432, false, 'Wiederholungart', 'Anzahl');
			}
		}

		// Zusammensetzen der Vorgangsnummer. Während $Vorgangsnummer fix ist, kann $Vorgang_naechster vom
		// User eingegeben werden, daher muss die endgültige Vorgangsnummer ($Vorgangsnummer_new) aus den
		// beiden zusammengesetzt werden
		$vi=(string)((int)trim($Vorgang_naechster));
		// Nur Ganzzahlen größer als 0 erlaubt
		if(($vi !== $Vorgang_naechster) || ($vi < 0))
		{
			$err=true;
			error_msg(1299, array('Vorgang_naechster'), '<B>'.$Vorgang_naechster.'</B>', 'Vorgangsnummer');	// 1299: Der Wert "$var1" den sie im Feld "$var2" eingegeben haben ist keine Ganzzahl. Versichern sie sich dass sie keine Buchstaben oder Punkte eingetragen haben.
		}
		else
		{
			$vnerr=check_vn_range($vi, array('Vorgang_naechster'));
			if($vnerr)
				$err=true;
			else
			{
				if($vPART == FXP_PRJ_MAIN)
					$Vorgangsnummer_new=$vi;
				else
					$Vorgangsnummer_new=$Vorgangsnummer.$vi;
				// Prüfen, ob Vorgangsnummer bereits belegt
				$vchk='';
				$vexp=explode('.', $Vorgangsnummer_new);
				foreach($vexp as $vtn)
				{
					if($vchk)
						$vchk .= '.';
					$vchk .= substr('0000'.(int)$vtn, -4);
				}
				$csql="SELECT projekt_id FROM projekte WHERE vorgangsnummer='$vchk' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				if($vAPID)
					$csql .= " AND projekt_id<>".$vAPID;
				$vexists=db_value($csql);
				if($vexists)
				{
					$err=true;
					error_msg(599, array('Vorgang_naechster'), '<B>'.$Vorgangsnummer_new.'</B>');	// 599: Ein Vorgang mit der Nummer [$var1] existiert bereits! Bitte wählen Sie eine andere Nummer aus.
				}
			}
		}

		// TERMINE
		// -------
		// Der Endzeitpunkt eines Vorgangs darf nicht vor dem Beginn liegen
		if($Soll_Beg_DtmDB > $Soll_End_DtmDB)
		{
			$err=true;
			error_msg(651, array("Soll_Beg_Dtm", "Soll_End_Dtm"), "Soll_Beg_Dtm", "Soll_End_Dtm", " ");
		}
		if(($Aufwand_Soll_up > $Max_Zeit_Aufw_up) && $Max_Zeit_Aufw_up && ($vPART == FXP_PRJ_TASK))
		{
			$err=true;
			error_msg(651, array("Aufwand_Soll", "Max_Zeit_Aufw"), "Aufwand_Soll", "Max_Zeit_Aufw", " ");
		}

		// Das geplante Enddatum darf nicht nach der Deadline liegen. Es wird mit nicht-
		// validierten Werten gearbeitet, da Deasline_up='NULL' wäre, wenn Deadline= ""
		if($Deadline && (substr($Soll_End_DtmDB, 0, 8) > substr($maskenfeldwerte['deadline'], 0, 8)))
		{
			$err=true;
			error_msg(651, array("Deadline", "Soll_End_Dtm"), "Deadline", "Soll_End_Dtm", " ");
		}

		// Budgets prüfen, ob diese im Rahmen liegen etc.
		if(sizeof($budgetrahmen))
		{
			$pdat=array(
				'aktion'				=> $GLOBALS['fxpglobals']['action'],
				'elter'					=> $vEPID,
				'projektart'			=> $vPART,

				'budgetrahmen_pers_int'	=>	$budgetrahmen_pers_int_up,
				'budgetrahmen_pers_ext'	=>	$budgetrahmen_pers_ext_up,
				'budgetrahmen_mat_int'	=>	$budgetrahmen_mat_int_up,
				'budgetrahmen_mat_ext'	=>	$budgetrahmen_mat_ext_up,

				'budget_pers_int_gepl'	=>	$budget_pers_int_gepl_up,
				'budget_pers_ext_gepl'	=>	$budget_pers_ext_gepl_up,
				'budget_mat_int_gepl'	=>	$budget_mat_int_gepl_up,
				'budget_mat_ext_gepl'	=>	$budget_mat_ext_gepl_up
			);
			$rerr=validate_budgetrahmen(1, $budgetrahmen, $pdat, $iea, $pma);
			if(fxIsArray($rerr))
			{
				$err=true;
				foreach($rerr as $merr)
				{
					eval($merr);
				}
			}
		}

		// Prüfung, ob der Vorgang innerhalb des zeitlichen Rahmens des Mutterprojektes liegt
		// Diese Prüfung entfällt für als Projekte definierte Vorgänge, da sie ihre eigenen Mutterprojekte darstellen
		if(($vPART != FXP_PRJ_MAIN) && $vEPID)
		{
			$anpassung='';

			// Der Vorgang auch im Rahmen seines Elters sein
			$tsql="SELECT soll_beginn, soll_ende FROM zeitdaten WHERE projekt_id=".$vEPID." AND zeitart=102";
			$daten=db_values($tsql);
			if(($daten['soll_beginn'] > $Soll_Beg_DtmDB) && (!$stretch_s || !$_POST['Button_Anpassen']) && $Soll_Beg_Dtm)
			{
				// Ist Einstellung gesetzt, Ja, Fehlermeldung bei Überschreitung des Elterrahmens => Fehler
				if(substr(strtolower($GLOBALS['fxpglobals']['settings']['p_versch_abfr']), 0, 1) == 'j')
					$err=true;

				// Enthält alle übergeordneten Vorgänge des aktuellen Vorgangs
				// Es muss der Elter als Parameter übergeben werden, da beim Anlegen der Vorgang
				// noch nicht physisch in der DB besteht, außerdem könnte er auch gerade verschoben werden
				$upper=superior_vn($vEPID, true);
				// Findet alle übergeordenten Vorgänge dieses Vorgangs die einen Vorgänger (Abhängigkeiten) haben
				$tsql="SELECT p.abhaengigkeit FROM projekte p, zeitdaten z WHERE p.projekt_id IN (".$upper.") AND abhaengigkeit<>0 AND z.soll_beginn>'".$Soll_Beg_DtmDB."' AND p.zeit_id=z.zeit_id";
				$stretch_abh=db_values($tsql, '*');

				// Verschiebung bei Abhängigkeiten ist derzeit nicht aktiv => Fehlermeldung
				if($stretch_abh)
				{
					$err=true;
					$anpassung=' '.meldung(2094, true);	// 2094: Ein automatisches Anpassen des Zeitrahmens der Elter in diese Richtung ist nicht möglich da Abhängigkeitsbeziehungen verletzt werden würden.
				}

				// Verschieben ohne Meldung
				if((substr(strtolower($GLOBALS['fxpglobals']['settings']['p_versch_abfr']), 0, 1) == 'n') && !$pers_in_abh && !$stretch_abh)
				{
					$_POST['Button_Anpassen']=true;
					$stretch_s=false;
					$stretch_OKs=true;
				}
				// Verschieben mit Meldung
				else
				{
					if(!$anpassung)
					{
						$anpassung=' '.meldung(2093, true);	// 2093: Ein automatisches Anpassen des Zeitrahmens der Elter in diese Richtung ist möglich.
						$stretch_s=true;						// Nur true wenn Anpassung auch möglich ist
					}
					// 1109: Der Termin "$var1" dieses Vorgangs liegt außerhalb des zeitlichen Rahmens der in seinem $var2 definiert wurde. Der Rahmen erstreckt sich nur vom $var3 bis zum $var4.$var5
					error_msg(1109, false, 'Soll_Beg_Dtm', 'Elter', get_entry($daten['soll_beginn'], 'datum'), get_entry($daten['soll_ende'], 'datum'), $anpassung);
				}
			}
			// Ist der Endzeitpunkt des Vorgangs nach dem des Elters ---> Fehler
			if(($Soll_End_DtmDB > $daten['soll_ende']) && (!$stretch_e || !$_POST['Button_Anpassen']) && $Soll_End_Dtm)
			{
				// Ist Einstellung gesetzt, Ja, Fehlermeldung bei Überschreitung des Elterrahmens => Fehler
				if(substr(strtolower($GLOBALS['fxpglobals']['settings']['p_versch_abfr']), 0, 1) == 'j')
					$err=true;

				// Enthält alle übergeordneten Vorgänge des aktuellen Vorgangs
				// Es muss der Elter als Parameter übergeben werden, da beim Anlegen der Vorgang
				// noch nicht physisch in der DB besteht, außerdem könnte er auch gerade verschoben werden */
				$upper=superior_vn($vEPID, true);
				// Findet alle übergeordenten Vorgänge dieses Vorgangs die einen Nachfolger (Abhängigkeiten) haben
				$tsql="SELECT p.projekt_id FROM projekte p, zeitdaten z WHERE p.abhaengigkeit IN (".$upper.") AND z.soll_beginn<'".$Soll_End_DtmDB."' AND p.zeit_id=z.zeit_id";
				$stretch_follow=db_values($tsql, '*');

				// Verschiebung bei Abhängigkeiten ist derzeit nicht aktiv => Fehlermeldung
				if($stretch_follow)
				{
					$err=true;
					$anpassung=' '.meldung(2094, true);	// 2094: Ein automatisches Anpassen des Zeitrahmens der Elter in diese Richtung ist nicht möglich da Abhängigkeitsbeziehungen verletzt werden würden.
				}
				// Verschieben ohne Meldung
				if((substr(strtolower($GLOBALS['fxpglobals']['settings']['p_versch_abfr']), 0, 1) == 'n') && !$pers_in_abh && !$stretch_follow)
				{
					$_POST['Button_Anpassen']=true;
					$stretch_e=false;
					$stretch_OKe=true;
				}
				// Verschieben mit Meldung
				else
				{
					if(!$anpassung)
					{
						$anpassung=' '.meldung(2093, true);	// 2093: Ein automatisches Anpassen des Zeitrahmens der Elter in diese Richtung ist möglich.
						$stretch_e=true;					// Nur true wenn Anpassung auch möglich ist
					}
					// 1109: Der Termin "$var1" dieses Vorgangs liegt außerhalb des zeitlichen Rahmens der in seinem $var2 definiert wurde. Der Rahmen erstreckt sich nur vom $var3 bis zum $var4.$var5
					error_msg(1109, false, 'Soll_End_Dtm', 'Elter', get_entry($daten['soll_beginn'], 'datum'), get_entry($daten['soll_ende'], 'datum'), $anpassung);
				}
			}
			protectVar($stretch_s, 'stretch_s');
			protectVar($stretch_e, 'stretch_e');
		}
	}

	// Prüfung der terminlichen Abhängigkeit von anderen Projekten
	if($Abhaengigkeit)
	{
		// Enddatum des abhängigen Vorgängerprojektes ermitteln (Endzeitpunkt=24:00 Uhr)
		$abh_End_Dtm=db_value("SELECT soll_ende FROM zeitdaten z, projekte p WHERE p.projekt_id=".$Abhaengigkeit." AND p.zeit_id=z.zeit_id");
		// Ist das Enddatum später (d.h. größer) als der Beginn (Startzeitpunkt=00:00 Uhr) des neuen Projekts?
		if($Soll_Beg_DtmDB < $abh_End_Dtm)
		{
			// ja, dann Meldung 638: Das Projekt, von dem dieses abhängig ist, ist zum Startzeitpunkt dieses Projektes noch nicht abgeschlossen. Der frühestmögliche Termin wäre der "$var1". $var2
			error_msg(638, -2, 'Soll_Beg_Dtm', get_entry(time_add($abh_End_Dtm, 1, 'D'), 'datum'), '');
			$err=true;
		}
	}

	// Nachfolger verschieben?
	if($Projekt_ID && ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
	{
		// Vollständige Abhängigkeitskette ermitteln
		$dependents=get_dependencies($Projekt_ID);

		// Auch Untervorgänge der abhängigen Projekte ermitteln
		if(fxIsArray($dependents))
		{
			$former_end=db_value("SELECT soll_ende FROM zeitdaten WHERE projekt_id=".$Projekt_ID." AND zeitart=102");
			$lag=time_diff($former_end, $Soll_End_DtmDB, 'S');

			//Für den Fall dass ein Projekt verschoben wird, von dem etwas abhängt
			// ... es werden nur noch die DB-Calls generiert, ausgeführt werden diese dann in der DB-Datei!
			if($lag > 0)
			{
				$y=0;
				foreach($dependents as $pid => $dmy)
				{
					// Bei diesen SQL-Befehlen werden letzendlich die Starttermin derer Projekte 
					// ermittelt, die von diesem abhängig sind. Die jeweiligen Termine stehen dann in 
					// der Variablen $Beginn.
					$Beginn=db_value("SELECT soll_beginn FROM zeitdaten WHERE zeitart=102 AND projekt_id=".$pid);

					// Hier werden die Starttermine der Projekte mit dem neuen Enddatum des aktuellen
					// Projekts verglichen. Ist der Endtermin des aktuellen Projektes gleich
					// später (d.h.größer) als der Starttermin der von diesem aktuellen Projekt abhängigen Projekte, 
					// folgt eine Warnmeldung und der mögliche Update/Insert Befehl 
					// wird nicht ausgeführt
					if(($Beginn < $Soll_End_DtmDB) && !$update)
					{
						$err=true;
						error_msg(637, array('Abhaengigkeit'));
						$update=true;
						// Beim allerersten Durchlauf steigt man hier aus
						if(!$y)
						{
							protectVar($update, 'update');
							break;
						}
					}
					// Abändern des Projektzeitrahmens des Projekts, das abhängig ist
					else if(($Beginn < $Soll_End_DtmDB) && $update)
					{
						$zeitrahmen=db_values("SELECT soll_beginn, soll_ende FROM zeitdaten WHERE zeitart=102 AND projekt_id=".$pid);
						$pro_dauer=time_diff($zeitrahmen['soll_beginn'], $zeitrahmen['soll_ende'], 'S');
						// Der neue Projektbeginn soll einen Tag nach dem Projektende des Vorgängers sein
						$neu_Beginn=time_add($zeitrahmen['soll_beginn'], $lag, 'S');
						// Soll-Ende des zu verschiebenden Projekts wird der neue beginn + die frühere Projektdauer			
						$neu_Soll_End=time_add($neu_Beginn, $pro_dauer, 'S');
						$query_verschiebung="UPDATE zeitdaten SET soll_beginn='".$neu_Beginn."', soll_ende='".$neu_Soll_End."' WHERE zeitart=102 AND projekt_id=".$pid;
						$query_verschiebung_chk="SELECT projekt_id FROM zeitdaten WHERE zeitart=102 AND projekt_id=".$pid;

						// Calls abspeichern => werden in db-Datei ausgeführt
						$call_for_DB[]=$query_verschiebung;
						$check_for_DB[]=$query_verschiebung_chk;
					}
					// Speichern der Steuervariable => heißt, Warnung wurde ausgegeben => bei erneutem Klick => Calls werden ausgeführt
					protectVar($update, 'update');

					$y++;
				}
			}
		}
	}

	// Vorgangsnummer in DB-Format XXXX.XXXX.XXXX
	$VN_arr=explode('.', $Vorgangsnummer_new);
	$Vorgangsnummer_new='';
	foreach($VN_arr as $vtn)
	{
		if($Vorgangsnummer_new)
			$Vorgangsnummer_new .= '.';
		$Vorgangsnummer_new .= substr('0000'.(int)$vtn, -4);
	}
	// und zurück in Maskenfeldwerte schreiben
	$maskenfeldwerte['vorgangsnummer']=$Vorgangsnummer_new;
//$err=true;
}
?>