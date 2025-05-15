<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : ma_verfuegbarkeit_ini.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-12-10                                                   //
// Last Change : 2024-07-05                                                   //
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
 * PF 43: Assign Person Ressources (HR)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

setForm(false);

// Kalenderwerte leeren
$GLOBALS['fxptdata']['cal']=array();
$awerr=0;
$maske181_sf=true;
$cpo43=true;
$today=fxNow();

$global_durchschnitt_ek=false;
$global_durchschnitt_vk=false;
$global_durchschnitt_aufw=false;

$use_transaction=true;
if($GLOBALS['fxpglobals']['tr'] == 27)	// Appointment Planner
	$use_transaction=false;

// Kommt der User von den Vorgänge (Detail), der Projektübersicht oder direkt vom Projektpopup?
$fehler=false;
$spid=0;
if($GLOBALS['gproject'])
	$spid=$GLOBALS['gproject'];
else if($_GET['Projekt_ID'])
	$spid=(int)$_GET['Projekt_ID'];
else if(($_GET['uid'] == 'newtr') && $GLOBALS['fxpglobals']['project'])
	$spid=$GLOBALS['fxpglobals']['project'];
if($spid)
{
	$ps=(int)db_value("SELECT projekt_status FROM projekte WHERE projekt_id=".$spid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if(($ps != FXP_PS_PLANNED) && ($ps != FXP_PS_ACTIVE))
		$spid=0;
}
if($spid)
{
	// Standardvorbelegungen
	$_POST['Projekt_ID']			= array(0=>$spid);
	$_POST['Personen_ID']			= array(0=>0);	// Alle Mitarbeiter
	$_POST['Abt_ID']				= 0;			// Gesamtabteilung
	$_POST['MaFaehigkeit_ID']		= 0;			// *aus Projektinfos*
	$_POST['Kalenderanzeigemodus']	= 0;			// Kalenderanzeige "automatisch"
	$_POST['Zeitspanne_von']		= '';			// *aus Projektinfos*
	$_POST['Zeitspanne_bis']		= '';			// *aus Projektinfos*

	$_POST['ZZeitProjekt']			= 1;			// Zeit aus Vorgang übernehmen
	$_POST['nur_plandaten']			= '';			// Ist-Daten berücksichtigen
	$_POST['nur_zugeordnet']		= '';			// Alle Personen berücksichtigen

	$_POST['ZAuslastung']			= 1;			// Nur Auslastung anzeigen
	$_POST['ZProjekte']				= '';			// Projekte nicht anzeigen

	$_POST['atyp_akt']				= 1;			// Aktive Aufgaben anzeigen
	$_POST['atyp_gen']				= 1;			// Genehmigte Aufgaben anzeigen
	$_POST['atyp_gepl']				= '';			// Geplante Aufgaben ohne Genehmigung nicht anzeigen

	$_POST['wartung_anz']			= '';			// Wartungsaufgaben nicht anzeigen

	$_POST['Button_Anzeigen']		= true;			// Klick auf Button "Anzeigen" simulieren

	// Projektinfos holen
	$pinf_sql  = "SELECT p.projektart, p.maart_id, p.mafaehigkeit_id, z.soll_beginn, z.soll_ende";
	$pinf_sql .= " FROM projekte p, zeitdaten z";
	$pinf_sql .= " WHERE p.projekt_id=".$spid." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pinf_sql .= " AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$pinf=db_values($pinf_sql);
	if(fxIsArray($pinf))
	{
		// ### Zeitdaten
		$_POST['Zeitspanne_von']=get_entry($pinf['soll_beginn'], 'datum');
		$_POST['Zeitspanne_bis']=get_entry($pinf['soll_ende'], 'datum');
		// ### Fähigkeiten
		if($pinf['mafaehigkeit_id'])
			$_POST['MaFaehigkeit_ID']=$pinf['mafaehigkeit_id'].'f';
		else if($pinf['maart_id'])
			$_POST['MaFaehigkeit_ID']=$pinf['maart_id'];
	}
}

// Falls weder Auslastung noch Projekte angehakt ist - Auslastung setzen
if(!$_POST['ZAuslastung'] && !$_POST['ZProjekte'])
	$_POST['ZAuslastung']=1;

// Einstellungen zwischenspeichern
if($_POST['Button_Anzeigen'])
{
	$GLOBALS['fxptdata']['backup']['Projekt_ID']			= $_POST['Projekt_ID'];
	$GLOBALS['fxptdata']['backup']['Personen_ID']			= $_POST['Personen_ID'];
	$GLOBALS['fxptdata']['backup']['Abt_ID']				= $_POST['Abt_ID'];
	$GLOBALS['fxptdata']['backup']['Zeitspanne_von']		= $_POST['Zeitspanne_von'];
	$GLOBALS['fxptdata']['backup']['Zeitspanne_bis']		= $_POST['Zeitspanne_bis'];
	$GLOBALS['fxptdata']['backup']['MaFaehigkeit_ID']		= $_POST['MaFaehigkeit_ID'];
	$GLOBALS['fxptdata']['backup']['Kalenderanzeigemodus']	= $_POST['Kalenderanzeigemodus'];

	$GLOBALS['fxptdata']['backup']['ZZeitProjekt']			= $_POST['ZZeitProjekt'];
	$GLOBALS['fxptdata']['backup']['nur_plandaten']			= $_POST['nur_plandaten'];
	$GLOBALS['fxptdata']['backup']['nur_zugeordnet']		= $_POST['nur_zugeordnet'];

	$GLOBALS['fxptdata']['backup']['ZAuslastung']			= $_POST['ZAuslastung'];
	$GLOBALS['fxptdata']['backup']['ZProjekte']				= $_POST['ZProjekte'];

	$GLOBALS['fxptdata']['backup']['atyp_akt']				= $_POST['atyp_akt'];
	$GLOBALS['fxptdata']['backup']['atyp_gen']				= $_POST['atyp_gen'];
	$GLOBALS['fxptdata']['backup']['atyp_gepl']				= $_POST['atyp_gepl'];

	$GLOBALS['fxptdata']['backup']['wartung_anz']			= $_POST['wartung_anz'];

	$GLOBALS['fxptdata']['backup']['Button_Anzeigen']		= true;

	unset($_POST);
	$_POST=$GLOBALS['fxptdata']['backup'];
	extract($_POST);
}

// Einstellungen aus Backup holen
else if(!$_POST['MAZ'] && fxIsArray($GLOBALS['fxptdata']['backup']))
{
	$_POST=$GLOBALS['fxptdata']['backup'];
	extract($_POST);
}
else if(fxIsArray($_POST))
	extract($_POST);

// Farblegende-Infos holen (Texte, Farbnamen, etc.)
if(!fxIsArray($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]))
	farblegende('vars');
extract($GLOBALS['fxptdata']['backup']['legend'.$GLOBALS['fxpglobals']['lang']]);

// Vorbelegung der Felder für die Maske (#181)
// -------------------------------------------------------------------------
// Startdatum:	Keine Benutzereingaben, dann als Startdatum den ersten
//				Wochentag der akt. Kalenderwoche
if(($_POST['Zeitspanne_von'] === NULL) || !strlen((string)$_POST['Zeitspanne_von']))
{
	// Startdatum=Starttag der aktuellen Woche
	$von=$today;

	$tag=(int)substr((string)$von, 6,2);
	$monat=(int)substr((string)$von, 4,2);
	$jahr=(int)substr((string)$von, 0,4);
	$wtg=fx_date('w', fx_mktime(0,0,0, $monat,$tag,$jahr));
	$stg=(int)($GLOBALS['fxpglobals']['settings']['ersterwt']);
	$von=fx_date('Ymd000000', fx_mktime(0,0,0, $monat,$tag-$wtg+$stg,$jahr));
}
// Startdatum: Benutzereingaben vorhanden, d.h. Datum auf Fehler prüfen
else
	$von=checkdatum('Zeitspanne_von');

// Enddatum:	Keine Benutzereingaben, dann Enddatum=Anzahl der Tage
//				entspr. den Benutzereinstellungen
if(($_POST['Zeitspanne_bis'] === NULL) || !strlen((string)$_POST['Zeitspanne_bis']))
{
	// Enddatum=Endtag nach der Standard-Zeitspanne (4 Wochen)
	$jahr=(int)(substr((string)$von, 0,4));
	$monat=(int)(substr((string)$von, 4,2));
	$tag=(int)(substr((string)$von, 6,2));
	$bis=fx_date('Ymd240000', fx_mktime(0,0,0, $monat,$tag+$GLOBALS['fxpglobals']['settings']['stdzeitspanne']-1,$jahr));
}
// Enddatum: Benutzereingaben vorhanden, d.h. Datum auf Fehler prüfen
else
	$bis=checkdatum('Zeitspanne_bis');

// Fehlerprüfung, ob Enddatum vor Beginndatum liegt
if(($von == '') || ($bis == ''))
{
	error_msg(1120, false); // Keine gültigen Zeitdaten
	$fehler=true;
}
else if($bis < $von)
{
	error_msg(1065, false); // Endzeit vor Startzeit
	$fehler=true;
}

if($fehler)
	$err=true;
else
	$bis=substr((string)$bis, 0, 8).'240000';

if(!fxIsArray($rpid))
{
	if(!fxIsArray($_POST['Projekt_ID']) && !isset($_POST['MAZ']))
		$rpid[0]=$GLOBALS['fxpglobals']['project'];	// Projekt aus Menubaum ausgewählt
	else
		$rpid=$_POST['Projekt_ID'];

	if(!$rpid[0])
		$rpid[0]=0;
}
//fxDebug($rpid, '$rpid');

// Projekt-ID
$Projekt_ID_arg=array
(
	'with_tasks'			=> true,
	'rp_projektstatus'		=> '297,300'	// geplant oder aktiv
);
$con=0;
$cus=0;
if($GLOBALS['fxpglobals']['persdat']['personentyp'] == FXP_PT_CUSTOMER_CP)
{
	$con=$GLOBALS['fxpglobals']['person'];
	$cus=$GLOBALS['fxpglobals']['persdat']['elter'];
}
// Nur der PL darf Ressourcenplanung machen...
// ...ausser eingeloggte Person ist eine Auftraggeber
if($con)
{
	$Projekt_ID_arg['rp_kunde']=$cus;
	$Projekt_ID_arg['rp_ansprechpartner']=$con;
}
// ...oder hat die Fähigkeit "Geschäftsleitung" oder "Ressourcenplanung"
else if(!$GLOBALS['fxpglobals']['persdat']['roles'][-3] && !$GLOBALS['fxpglobals']['persdat']['roles'][2])
	$Projekt_ID_arg['rp_projektleiter']=$GLOBALS['fxpglobals']['person'];
//fxDebug($Projekt_ID_arg, '$Projekt_ID_arg');
$spa=auswahl_Projekt_ID(1);
//fxDebug($spa, '$spa');

// Prüfungen nur bei direktem Aufruf der Ressourcenplanung
if($GLOBALS['fxpglobals']['tr'] == 43)
{
	// Prüfung, ob vorselektierte Aufgabe im Auswahlfeld enthalten ist
	if(fxIsArray($spa) && fxIsArray($rpid))
	{
		foreach($rpid as $cpid)
		{
			if($cpid && !isset($spa[$cpid]))
			{
				$sql="SELECT vorgangsnummer, name_projekt FROM projekte WHERE projekt_id=".$cpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$cpi=db_values($sql);
				if(fxIsArray($cpi))
				{
					error_msg(2825, false, '&quot;<b>'.no_null($cpi['vorgangsnummer']).': '.$cpi['name_projekt'].'</b>&quot;');	// <b>Achtung:</b> Der Vorgang $var1 befindet sich nicht im Vorgangsauswahlmenü und deshalb kann darauf keine Ressourcenzuordnung erfolgen! Bitte prüfen Sie Ihre Filtereinstellungen oder Rechte.
					$awerr=true;
					$_POST['Projekt_ID']=array(0=>0);
					break;
				}
			}
		}
	}

	// Prüfung, ob inaktive oder abgeschlossene Aufgaben gewählt wurden
	$igs=false;
	if(isset($ignore_status))
		$igs=true;
	if(fxIsArray($rpid) && !$igs)
	{
		$rcsv=array_to_csv($rpid, false);
		$sc=array(FXP_PS_PLANNED=>0, FXP_PS_ACTIVE=>0, FXP_PS_INACTIVE=>0, FXP_PS_COMPLETED=>0);
		$sql="SELECT projekt_id, projekt_status FROM projekte WHERE projekt_id IN (".$rcsv.") AND projekt_id>0 AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$tmp=db_values($sql, '*');
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$ps=(int)$t['projekt_status'];
				if(($ps == FXP_PS_INACTIVE) || ($ps == FXP_PS_COMPLETED))
				{
					$id=(int)$t['projekt_id'];
					unset($rpid[$id]);
					if(fxIsArray($_POST) && isset($_POST['Projekt_ID']) && fxIsArray($_POST['Projekt_ID']))
					{
						foreach($_POST['Projekt_ID'] as $ppk => $ppv)
						{
							if($ppv == $id)
								unset($_POST['Projekt_ID'][$ppk]);
						}
					}
				}
				$sc[$ps]++;
			}
		}
//fxDebug($sc, '$sc');
		if($sc[FXP_PS_INACTIVE] || $sc[FXP_PS_COMPLETED])
		{
			error_msg(2813, false);	// <b>Achtung:</b> Auf inaktive oder abgeschlossene Vorgänge kann keine Ressourcenzuordnung erfolgen!
			$awerr=true;
		}
	}
}

if(!fxIsArray($_POST) || !fxIsArray($_POST['Projekt_ID']))
	$_POST['Projekt_ID']=array(0=>0);
$Projekt_ID=$_POST['Projekt_ID'];

// Projekt-IDs ermitteln
$assign_granted	= true;
if(!$awerr && (!isset($iau) || !strlen((string)$iau) || !isset($iau_arr) || !fxIsArray($iau_arr)))
{
	$iau='';
	$iau_arr=array();
	if(fxIsArray($rpid))
		$iau=$rpid[0];
	if(!strlen((string)$iau)|| ($iau == '-666') || !$iau)
	{
		$assign_granted=false;
		$iau=0;

		// Im Anzeigemodus Auslastung ermitteln, d.h. alle Projekte
		if($_POST['Button_Anzeigen'] && ($_POST['ZAuslastung'] || $_POST['ZProjekte']))
		{
			$sql=auswahl_Projekt_ID(-1)." AND p.projektart=".FXP_PRJ_TASK." AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
			if(!$igs)
				$sql .= " AND p.projekt_status IN (".FXP_PS_PLANNED.",".FXP_PS_ACTIVE.")";
			if(strlen((string)$von) && strlen((string)$bis))
				$sql .= " AND (z.soll_beginn<='".$bis."' OR z.soll_ende>='".$von."')";
			else if(strlen((string)$von))
				$sql .= " AND z.soll_beginn>='".$von."'";
			else if(strlen((string)$bis))
				$sql .= " AND z.soll_ende<='".$bis."'";
			$cnt_sql=str_replace("SELECT p.projekt_id", "SELECT COUNT(p.projekt_id)", $sql);
			$cnt=(int)db_value($cnt_sql);
			if($cnt)
				$iau=$sql;
		}
	}

	// Alle geplanten oder aktiven Aufgaben ermitteln
	$zsv='';
	$zsb='';
	if($iau)
	{
		$sql  = "SELECT p.projekt_id, p.projekt_status, p.projektart, p.aufwand_soll, p.name_projekt, p.vorgangsnummer,";
		$sql .= " z.soll_beginn, z.soll_ende";
		$sql .= " FROM projekte p, zeitdaten z";
		$sql .= " WHERE p.projekt_id IN (".$iau.") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " AND z.projekt_id=p.projekt_id AND z.zeitart=102 AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY p.vorgangsnummer";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$apid=(int)$t['projekt_id'];

				if(!$zsv || ($t['soll_beginn'] < $zsv))
					$zsv=$t['soll_beginn'];
				if(!$zsb || ($t['soll_ende'] > $zsb))
					$zsb=$t['soll_ende'];

				$iau_arr[$apid]=$t;
			}
		}
	}
	if($rpid[0] && !sizeof($iau_arr))
	{
		error_msg(1598, false);	// <B>Achtung!</B> Zu diesem Projekt/Teilprojekt gibt es (noch) keine untergeordneten geplanten oder aktiven Aufgaben!
		$awerr=true;
	}
}

echo('<div id="ntasks" style="position:absolute;left:0;top:-100px;display:none;">'.sizeof($iau_arr).'</div>'.$nl);
if(sizeof($iau_arr) == 1)	// Nur eine Aufgabe
	$praufg=true;
else
	$praufg=false;

$mode=1;
if($_POST['nur_plandaten'])
	$mode=0;

if($_POST['ZZeitProjekt'] && $_POST['Projekt_ID'][0] && $zsv && $zsb)
{
	$von=substr((string)$zsv, 0, 8).'000000';
	$bis=substr((string)$zsb, 0, 8).'240000';
	if(($mode > 0) && ($bis < $today))
		$bis=substr((string)$today, 0, 8).'240000';
}

// Warnhinweis, falls Zuordnung zu mehreren Aufgaben
if(!$awerr && $assign_granted && $_POST['Button_Anzeigen'] && fxIsArray($iau_arr) && (sizeof($iau_arr) > 1))
	error_msg(1755, false);	// <B>Achtung:</B> Die Ressourcenzuordnung hat Auswirkung auf alle gewählten geplanten oder aktiven Aufgaben!

// Mitarbeiter zuweisen, wenn auf Button geklickt wurde
if(!$awerr && $assign_granted && $_POST['MAZ'] && $iau && sizeof($iau_arr))
{
	$_POST['Button_Anzeigen']=true;
	if(fxIsArray($_POST['AUFW']))
	{
		$r=new RESOURCE($use_transaction);
		if($r)
		{
//			$r->setDebugMode(255);
			$r->addPOST($iau_arr);
			$awerr=$r->assignResources();
			unset($r);
		}
		else
			$awerr=true;
	}
}

if($awerr)
	$err=true;
?>