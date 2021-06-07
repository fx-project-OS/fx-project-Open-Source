<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : stunden_tag_liste_ini.inc                                    //
// Version     : 21.1                                                         //
// Begin       : 2020-11-09                                                   //
// Last Change : 2021-03-01                                                   //
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
 * @version 21.1
 */

//fxDebug('_GET'); fxDebug('_POST');

$sum=false;
if(($GLOBALS['fxpglobals']['tr'] == 34) || ($tr == 34))
	$sum=true;

$rights34=tr_rechte(34);	// ZE (Summe)
$rights81=tr_rechte(81);	// ZE (Tag)
$rights150=tr_rechte(150);	// RK
$rights114=tr_rechte(114);	// Leistungsnachweis

// Personenauswahlmaske
$pers_id=0;

// Kommt der Benutzer über den Projektworkflow, dann gleich Aufgabe auswählen
if($GLOBALS['gproject'])
{
	$pers_id=$GLOBALS['fxpglobals']['person'];
	$_POST['Projekt_ID']=$GLOBALS['gproject'];
	$Kunde='';
	$ipd=substr($GLOBALS['datetime'],0,8);
}
else if(!sizeof($_POST) && (!sizeof($_GET) || ($_GET['uid'] != 'datenav')))
{
	$ipd=substr($GLOBALS['datetime'],0,8);
	$_POST['Projekt_ID']=$GLOBALS['fxpglobals']['project'];
}

// Müssen Start- und Endzeit eingegeben  werden?
if(strtolower($GLOBALS['fxpglobals']['settings']['z_zeit']) == 'ja')
	$z_zeit=true;
else
	$z_zeit=false;

// Muss ein Kunde ausgewählt werden?
if(strtolower($GLOBALS['fxpglobals']['settings']['z_kunde']) == 'ja')
	$z_kunde=true;
else
	$z_kunde=false;

// Muss Tätigkeit ausgefüllt werden?
if(strtolower($GLOBALS['fxpglobals']['settings']['z_taetigkeit']) == 'ja')
	$z_taetigkeit=true;
else
	$z_taetigkeit=false;

// Ist die Kostenrechnung aktiv?
if(function_exists('fxsppCheckEBTR') && fxsppCheckEBTR(241) && (substr(strtolower($GLOBALS['fxpglobals']['settings']['kr_aktiv']), 0, 1) == 'j'))	// Kostenrechnung aktiv
	$kr_aktiv=true;
else
	$kr_aktiv=false;

// Falls Kostenrechnung aktiv - Muss eine Leistungsart angegeben werden?
if(strtolower($GLOBALS['fxpglobals']['settings']['kr_lamuss']) == 'ja')
	$kr_lamuss=true;
else
	$kr_lamuss=false;

// Wurde eine Leistungsart oder ein ToDo ausgewählt, dann IDs splitten
$lart_id=0;
$todo_id=0;
if(isset($_POST['Projekt_ID']))
	$cpid=$_POST['Projekt_ID'];
else if(isset($_GET['Projekt_ID']))
	$cpid=$_GET['Projekt_ID'];
else if(isset($GLOBALS['fxptdata']['backup']['old_task']))
	$cpid=$GLOBALS['fxptdata']['backup']['old_task'];
else
	$cpid=$GLOBALS['fxpglobals']['project'];
if(strlen($cpid))
{
	$upos=strpos($cpid, '_');	// Leistungsart
	$mpos=strpos($cpid, '-');	// ToDo
	if($upos !== false)
	{
		$lart_id=(int)substr($cpid, $upos+1);
		$Projekt_ID=(int)substr($cpid, 0, $upos);
	}
	else if($mpos !== false)
	{
		$todo_id=(int)substr($cpid, $mpos+1);
		$Projekt_ID=(int)substr($cpid, 0, $mpos);
	}
	else
		$Projekt_ID=(int)$cpid;
}
//echo('$Projekt_ID='.$Projekt_ID.', $lart_id='.$lart_id.', $todo_id='.$todo_id.'<hr />');

// Standardtexte setzen
$unbekannt_str	= meldung(1449, false); // [Unbekannt]
$gleitzeit_str	= meldung(1525, false); // [Gleitzeit]
$km_str			= meldung(2277, false);	// km
$i_anzeigen		= get_text(1001, '10');	// Anzeigen
$tt_str			= array('aufwand_soll'=>get_text(297, '20'), 'aufwand_ist'=>get_text(298, '20'), 'deadline'=>get_text(48, '20'), 'max_zeit_aufw'=>get_text(132, '20'), 'std'=>meldung(916));
$unbekannt		= '~<i class="grey">'.$unbekannt_str.'</i>';
$gleitzeit		= '~<i class="grey>'.$gleitzeit_str.'</i>';
$include_all_masks=true;

// Wurde Zeiterfassung über ein anderes Programm aufgerufen?
if($_GET['tid'])
{
	$Taetigkeit_ID=$_GET['tid'];
	$stl_array=db_values("SELECT personen_id, projekt_id, zeit_id, taetigkeit_text, anmerkungintern FROM taetigkeiten WHERE taetigkeit_id=".$Taetigkeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if(fxIsArray($stl_array))
	{
		$Personen_ID		= $stl_array['personen_id'];
		$Projekt_ID			= $stl_array['projekt_id'];
		$Zeit_ID			= $stl_array['zeit_id'];
		$Taetigkeit_Text	= $stl_array['taetigkeit_text'];
		$Anmerkung			= $stl_array['anmerkungintern'];

		$GLOBALS['fxptdata']['backup']['personen_id']=$Personen_ID;

		$stl_array=db_values("SELECT ist_beginn, ist_ende, pause, zeitaufwand FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if(fxIsArray($stl_array))
		{
			$Ist_Beg_Zeit	= get_entry($stl_array['ist_beginn'], 'zeit');
			$Ist_End_Zeit	= get_entry($stl_array['ist_ende'], 'zeit');
			$Pause			= get_entry($stl_array['pause'], 'zeitspannet');
			$Zeitaufwand	= get_entry($stl_array['zeitaufwand'], 'zeitspannet');

			if($Ist_Beg_Zeit == $Ist_End_Zeit)
			{
				$Ist_Beg_Zeit='';
				$Ist_End_Zeit='';
			}
			else
				$Zeitaufwand='';

			if(isset($_GET['gd']))
				$_GET['gd']=substr($stl_array['ist_beginn'], 0, 8);
		}
	}
	$icon_aendern_x=1;
}
else
{
	if($pers_id)
		$GLOBALS['fxptdata']['backup']['personen_id']=$pers_id;
	else if(isset($_GET) && fxIsArray($_GET) && isset($_GET['persid']))
		$GLOBALS['fxptdata']['backup']['personen_id']=(int)$_GET['persid'];
	else if(($_GET['tr'] == 81) || ($_GET['tr'] == 34) || !isset($GLOBALS['fxptdata']['backup']) || !fxIsArray($GLOBALS['fxptdata']['backup']))
	{
		$GLOBALS['fxptdata']['backup']['personen_id']=$GLOBALS['fxpglobals']['person'];
		// Personenauswahl bei Geschäftsführer oder Admin, falls in Einstellungswerte so eingestellt
		if((strtolower($GLOBALS['fxpglobals']['settings']['gf_darf_zem']) == 'ja') && ($GLOBALS['fxpglobals']['persdat']['roles'][2] || ($GLOBALS['fxpglobals']['user'] == 1)))
			$GLOBALS['fxptdata']['backup']['personen_id']=0;
		// Personenauswahl bei Verwaltung, falls in Einstellungswerte so eingestellt
		if((strtolower($GLOBALS['fxpglobals']['settings']['vw_darf_zem']) == 'ja') && $GLOBALS['fxpglobals']['persdat']['roles'][-1])
			$GLOBALS['fxptdata']['backup']['personen_id']=0;
	}
	else if(isset($_POST) && fxIsArray($_POST) && isset($_POST['db_update']) && isset($_POST['Personen_ID']))
		$GLOBALS['fxptdata']['backup']['personen_id']=(int)$_POST['Personen_ID'];

	if($_GET['project'])
		$Projekt_ID=$_GET['project'];

	// Terminaufgaben: Sprung aus Kontrollzentrum
	if($_GET['gd'] && $_GET['ge'])
	{
		$GLOBALS['fxptdata']['backup']['personen_id']=$GLOBALS['fxpglobals']['person'];

		$fertig=1;
		if($z_zeit)
		{
			$Ist_Beg_Zeit=get_entry($_GET['gd'], 'zeit');
			$Ist_End_Zeit=get_entry($_GET['ge'], 'zeit');
		}
		else
		{
			$sek_beg=(int)substr($_GET['gd'], 8, 2)*3600 + (int)substr($_GET['gd'], 10, 2)*60 + (int)substr($_GET['gd'], 12);
			$sek_end=(int)substr($_GET['ge'], 8, 2)*3600 + (int)substr($_GET['ge'], 10, 2)*60 + (int)substr($_GET['ge'], 12);
			$Zeitaufwand=get_entry($sek_end-$sek_beg, 'zeitspannet');
		}
		if($z_taetigkeit)
			$Taetigkeit_Text=$GLOBALS['fxpglobals']['settings']['name_terminpr'].': '.get_entry($_GET['gd'], 'datetime').' - '.get_entry($_GET['ge'], 'zeit');
	}
}

// Personen_ID
$pers_id=$GLOBALS['fxptdata']['backup']['personen_id'];
$vb_pid=$pers_id;

// Monatsnamen
$monatsnamen=db_values("SELECT tabwert FROM wertetab WHERE id_feld=62 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');
// Wochentagsnamen
$wochentagsnamen=db_values("SELECT tabwert FROM wertetab WHERE id_feld=289 AND id_sprache=".$GLOBALS['fxpglobals']['lang']." AND mandanten_id=0 ORDER BY position", '*');

// Personenname ermitteln
if($vb_pid)
{
	$sql="SELECT pname, vorname, anrede FROM personen WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$arr=db_values($sql);

	// Falls aus der Datenbank der Name nicht gefunden wurde wird [Unbekannt] ausgegeben
	if(!fxIsArray($arr) && !strlen(trim($arr['pname'])))
	{
		$pname=meldung(681, false);
		$vname='';
		$anrede='';
	}
	else
	{
		$pname=trim($arr['pname']);
		$vname=trim($arr['vorname']);
		$anrede=meldung($arr['anrede'], false);
	}
	$personenname=$pname;
	if(strlen($vname))
		$personenname .= ', '.$vname;
}
else
	$personenname='Sysadmin';

// Prüfen, ob Datum eingegeben wurde
if($ipd)
{
	$Dummy=FXP_DISPLAY;
	$sipd=strlen($ipd);

	// Max. 2 Zeichen werden als Monatsangabe interpretiert
	if($sipd < 3)
	{
		$gm=(int)$ipd;
		if($gm < 1)
			$gm=1;
		if($gm > 12)
			$gm=12;
		$gm=substr($ad,0,4).substr('00'.$gm,-2).'00000000';
	}
	// Max. 4 Zeichen werden als Jahresangabe interpretiert
	else if($sipd < 5)
		$gj=(int)$ipd;
	// Alles andere entspr. der Validierungsfunktion als Tag interpretieren
	else
	{
		$gd=validate_entry($ipd, $derr, 'datum');
		$Dummy=FXP_CREATE;
	}
}

// Kalendervariable setzen
if($Button_OK)
{
	$dex=explode('|', $Dummy);
	$Dummy=(int)$dex[0];
	if($Dummy > FXP_CREATE)
	{
		$Zeit_ID=(int)$dex[1];
		$Taetigkeit_ID=(int)$dex[2];
	}
	if($sum)
	{
		if(!strlen($startdatum))
		{
			if($Zeit_ID)
				$gd=db_value("SELECT ist_beginn FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		}
		else
			$gd=validate_entry($startdatum, $derr, 'datum');
	}
	else if(!strlen($Datum))
	{
		if($Zeit_ID)
			$gd=db_value("SELECT ist_beginn FROM zeitdaten WHERE zeit_id=".$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	}
	else
		$gd=validate_entry($Datum, $derr, 'datum');
}

// Prüfen, ob auf Anzeigen-, Ändern- oder Löschen-Icon geklickt wurde
if(isset($icon_anzeigen_x) || isset($icon_aendern_x) || isset($icon_loeschen_x))
{
	if(isset($icon_anzeigen_x))
	{
		$Dummy=FXP_CHANGE;
		$db_update=false;
		$Button_OK_nv=true;
		$Button_Reset_nv=true;
		if($sum)
			$maske220_sf=1;
		else
			$maske218_sf=1;
	}
	else if(isset($icon_aendern_x))
	{
		unset($db_update);
		if($Datumsangabe) // Wochen- oder Monatsübersicht
		{
			$Dummy=FXP_CREATE;
			$gd=validate_entry($Datumsangabe, $derr, 'datum');
		}
		else
			$Dummy=FXP_CHANGE;
	}
	else
		$Dummy=FXP_DELETE;
//fxDebug($Dummy,'$Dummy', 0);

	if(($Dummy > FXP_CREATE) && $Zeit_ID)
	{
		$startdatum='';
		$enddatum='';

		$sql  = "SELECT z.ist_beginn, z.ist_ende, z.zeitaufwand, z.pause, z.projekt_id, z.zkunde, z.freigabe, z.fertig, z.fg_relativ, z.zeiteinheit, z.zeitart, t.taetigkeit_text, t.anmerkungintern";
		$sql .= " FROM zeitdaten z, taetigkeiten t WHERE z.zeit_id=".(int)$Zeit_ID." AND t.zeit_id=z.zeit_id AND t.mandanten_id=z.mandanten_id AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$adata=db_values($sql);
		if(fxIsArray($adata))
		{
			$gd					= $adata['ist_beginn'];
			$fertig				= $adata['fertig'];
			$fg_relativ			= $adata['fg_relativ'];

			$Projekt_ID			= $adata['projekt_id'];
			$Kunde				= $adata['zkunde'];

			$Zeitaufwand		= get_entry($adata['zeitaufwand'],'zeitspannet');

			$Taetigkeit_Text	= $adata['taetigkeit_text'];
			$Anmerkung			= $adata['anmerkungintern'];

			if($adata['zeitart'] != FXP_TE_TIMEREC_SUM)
			{
				$Freigabe		= $adata['freigabe'];

				$Ist_Beg_Zeit	= get_entry($adata['ist_beginn'],'zeit');
				$Ist_End_Zeit	= get_entry($adata['ist_ende'],'zeit');
				$Pause			= get_entry($adata['pause'],'zeitspannet');
			}
			else if($adata['zeiteinheit'] == (int)$Zeit_ID)
			{
				$sql="SELECT zeit_id,projekt_id,ist_beginn,zeitaufwand,freigabe,rechnungs_id FROM zeitdaten WHERE zeiteinheit=".(int)$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY ist_beginn";
				$tmp=db_values($sql,'*');
//fxDebug($tmp,$sql);
				$startdatum=substr($tmp[0]['ist_beginn'],0,8);
				$zaw=0;
				$acc=true;
				foreach($tmp as $t)
				{
					if(($t['rechnungs_id']) || ($t['freigabe'] && ($t['freigabe'] > ($GLOBALS['fxpglobals']['persdat']['role']+1))))
						$acc=false;

					$zaw += $t['zeitaufwand'];
					$enddatum=substr($t['ist_beginn'],0,8);
				}
				if(!$acc)
				{
					$zaw=$tmp[0]['zeitaufwand'];
					$enddatum='';
				}
				$Zeitaufwand=get_entry($zaw,'zeitspannet');
				if($enddatum == $startdatum)
					$enddatum='';
			}
		}
		$lart=db_value("SELECT kostenstelle_id FROM kostenzuordnung WHERE id=".(int)$Zeit_ID." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND kostenstellenart=".FXP_TE_TIMEREC." AND kostendefinition=903");
		if($lart)
			$lart_id=(int)$lart;

		$Datum=get_entry($gd,'datum');

		if(!strlen($startdatum))
			$startdatum=$Datum;

		if($Ist_Beg_Zeit == $Ist_End_Zeit)
		{
			$Ist_Beg_Zeit='';
			$Ist_End_Zeit='';
		}
		else
			$Zeitaufwand='';
	}
}
$GLOBALS['fxptdata']['backup']['old_task']=$Projekt_ID;

if(!$Kunde && !$Projekt_ID && isset($GLOBALS['fxptdata']['backup']['old_cust']))
	$Kunde=$GLOBALS['fxptdata']['backup']['old_cust'];
$GLOBALS['fxptdata']['backup']['old_cust']=$Kunde;

// Datum zuweisen
$heute=substr($GLOBALS['datetime'],0,8);
if($_GET['gd'])
{
	$datum=$_GET['gd'];
	if($z_zeit && (strlen($datum) > 8))
	{
		$dzeit=substr('000000'.substr($datum, 8), -6);
		$datum=substr($datum.'000000', 0, 14);
		if($dzeit != '000000')
			$Ist_Beg_Zeit=get_entry($datum, 'zeit');
	}
}
else if($gd)
	$datum=$gd;
else if($gw)
	$datum=substr($gw, 0, 6).substr($gw, 10, 2);
else if($gm)
	$datum=substr($gm, 0, 6).'01';
else if($gj)
	$datum=substr($gj, 0, 4).'0101';
else
{
	if(strlen($GLOBALS['fxptdata']['backup']['old_date']) && !$_POST['Person_waehlen'])
		$datum=$GLOBALS['fxptdata']['backup']['old_date'];
	else
		$datum=$heute.'000000';
	$gd=$datum;
}
if(substr($datum, 0, 1) != '^')
	$datum=substr($datum, 0, 8).'000000';
if($gd && (substr($datum, 6, 2) == '00'))
{
	$datum=$heute.'000000';
	$gd=$datum;
}
$GLOBALS['fxptdata']['backup']['old_date']=$datum;

if($z_zeit && $_GET['et'])
{
	if(strlen($_GET['et']) > 6)
		$Ist_End_Zeit=get_entry(substr($_GET['et'].'00000000000000',0,14), 'zeit');
	else
		$Ist_End_Zeit=get_entry(substr($datum,0,8).substr($_GET['et'].'000000',0,6), 'zeit');
}

if($sum)
{
	$startdatum=$datum;
	if(!isset($Button_OK) && !isset($icon_anzeigen_x) && !isset($icon_aendern_x) && !isset($icon_loeschen_x))
		$enddatum='';
}

// Eintritts- + Austrittsdatum ermitteln
$maeintrittsdatum='';
$maaustrittsdatum='';
if($vb_pid)
{
	$eadaten=db_values("SELECT maeintrittsdatum, maaustrittsdatum FROM madaten WHERE personen_id=".$vb_pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
	if(fxIsArray($eadaten))
	{
		$maeintrittsdatum=substr($eadaten['maeintrittsdatum'], 0,8);
		if(strlen($eadaten['maaustrittsdatum']))
			$maaustrittsdatum=substr($eadaten['maaustrittsdatum'], 0,8);
		else
			$maaustrittsdatum='99999999';
	}
}

// Felder vorbereiten, d.h. Muss-Felder, Felder ausblenden etc.
if(!$sum)
{
	$Datum_muss=true;
	if($z_zeit)
	{
		$Ist_Beg_Zeit_muss=true;
		$Ist_End_Zeit_muss=true;
	}
	else
		$Zeitaufwand_muss=true;
}
if($z_kunde)
	$Kunde_muss	= true;
if($z_taetigkeit)
	$Taetigkeit_Text_muss=true;

// Falls Person bereits feststeht, dann Auswahlmaske ausblenden
$maske184_nv=false;
if($pers_id)
	$maske184_nv=true;
else
{
	$gd='';
	$Personen_ID_arg['modus']=0;
	$Pers_Arr=auswahl_Personen_ID(1);
	$Personen_ID_arg=array('override'=>$Pers_Arr);
	if(fxIsArray($GLOBALS['pid_tr']) && ($GLOBALS['pid_tr']['a'] < $GLOBALS['pid_tr']['b']))
		error_msg(2996, false, '<b>'.($GLOBALS['pid_tr']['b']-$GLOBALS['pid_tr']['a']).'</b>', $GLOBALS['pid_tr']['b'], 'Personen_ID');
}

// Hinweis bzgl. Ein-/Austrittsdatum?
$sd=$startdatum;
if(!strlen($sd))
	$sd=$datum;
if(strlen($sd))
	$sd=validate_entry($sd, $derr, 'datum');
$ed=$enddatum;
if(strlen($ed))
	$ed=validate_entry($ed, $derr, 'datum');
else
	$ed=$sd;
warnTimeRecordingDates($pers_id, $sd, $ed);

// Zu Beginn eine grosse Tabelle anlegen
if(!isset($GLOBALS['iname_array']))
{
	$tdstyle='';
	if($pers_id)
		$tdstyle=' style="padding-bottom:12px;border-bottom:1px dotted #cccccc;"';

	echo('				<table border="0" cellpadding="0" cellspacing="0">'.$nl);
	echo('					<tr>'.$nl);
	echo('						<td nowrap width="100%" valign="top"'.$tdstyle.'>'.$nl);
}
?>