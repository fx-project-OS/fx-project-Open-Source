<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : telefonliste_pre.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2020-12-23                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 197: Phone Book
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$tl_debug=false;
$inakt=' <i class="lightergrey">{'.get_text(1391, '20').'}</i>';	// {inaktiv}

// Daten nur ermitteln, wenn auf den Button gedrückt wurde
if(fxIsArray($_POST) && isset($_POST['Suchtext']) && isset($_POST['Button_suchen']))
{
	// Kein Zeilenumbruch bei Namen
	$P_Name_nw=true;

	// In dieser Maske wird die Telefonliste dargestellt
	$maskenzaehler=2;

	// Filter (ohne Personenrechtbeschränkungen)
	$flt=filter_person('and', true, false, 'Personen_ID', false);
	if(strlen((string)$flt))
		$flt=" ".$flt;
	$maske406_flt=$mskfilter;
//fxDebug($flt,'$flt: $mskfilter='.$mskfilter, 0);

	// Wurde eine Abteilung ausgewählt?
	$sql_zusatz=$flt;
	if($_POST['Abt_ID'])
		$sql_zusatz .= " AND p.abt_id=".$_POST['Abt_ID'];

	// Rechte prüfen
	$check=true;
	if(!$GLOBALS['fxpglobals']['person'] || $GLOBALS['fxpglobals']['persdat']['roles'][-1] || $GLOBALS['fxpglobals']['persdat']['roles'][2])		// Verwaltung (-1) und Geschäftsführer (2) dürfen alle Infos sehen
		$check=false;
//fxDebug($check,'$check', 0);
	if($check)
	{
		$sql_zusatz .= " AND (";

		// Nur spez. Personentyp anzeigen, falls Transaktionsrecht
		$ptypr=personentyp_rechte();
		$sql_zusatz .= "p.personentyp IN (".$ptypr.")";

		// Nur Personen anzeigen, auf die Recht vorhanden ist
		$sql_zusatz .= " OR p.personen_id IN (".get_persons().")";

		$sql_zusatz .= ")";
	}

	// Standard-SQL
	$sql_std  = "SELECT p.pname AS p_name, p.vorname, p.personen_id, p.personentyp, p.geschlecht, p.inaktiv,";
	$sql_std .= " k.kommunikation_id, k.kommunikationstyp AS typ, k.kommunikation_txt AS telefonnummer";
	$sql_std .= " FROM personen p, kommunikation k";
	$sql_std .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND k.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND k.personen_id=p.personen_id";

	// Alle Namen und Telefonnummern aus der Tabelle Personen+Kommunikation ermitteln, für die komplette Rechte vorhanden sind
	$sql=$sql_std.$sql_zusatz." ".db_orderby('p.pname', 'p.vorname');
	$l1=db_values($sql, '*');
//fxDebug($l1, $sql, 0);

	// Alle Namen und Standardnummern aus der Tabelle Personen+Kommunikation ermitteln, für die keine Rechte vorhanden sind
	// 1872=Fax, 1873=Mobil, 2365=Tel, 2366=Email, 2367=Web
	$pcsv="0";
	if(fxIsArray($l1))
	{
		$prem=array();
		foreach($l1 as $l)
		{
			$pid=(int)$l['personen_id'];
			if(!isset($prem[$pid]))
			{
				$prem[$pid]=true;
				$pcsv .= ",".$pid;
			}
		}
	}
//fxDebug($pcsv, '$pcsv', 0);

	$sql=$sql_std.$sql_zusatz." AND p.personen_id NOT IN (".$pcsv.") AND k.kommunikationstyp IN (1872,1873,2365,2366,2367) ".db_orderby('p.pname', 'p.vorname');
	$l2=db_values($sql, '*');
//fxDebug($l2, $sql, 0);

	// Liste erstellen
	$liste=array();
	for($i=1; $i<3; $i++)
	{
		$li=${'l'.$i};
		if(fxIsArray($li))
		{
//fxDebug($li, $i);
			foreach($li as $l)
			{
				$id=str_replace(' ', '_', strtolower((string)$l['p_name'])).'|';
				if(!is_null($l['vorname']) && strlen((string)$l['vorname']))
					$id .= str_replace(' ', '_', strtolower((string)$l['vorname']));
				$id .= '|'.substr('0000000'.$l['typ'], -7).'-'.$l['kommunikation_id'];
				$liste[$id]=$l;
			}
		}
	}
	if(sizeof($liste))
		ksort($liste);
//fxDebug($liste,'$liste', 0);

	// Pattern für Name + Telefonnummer ermitteln
	$replace		= "<b class=\"red\">$0</b>";
	$name_pattern	= '';
	$phone_pattern	= '';
	if(strlen((string)$_POST['Suchtext']))
	{
		// Name
		$name_pattern=preg_quote(strtolower((string)trim((string)$_POST['Suchtext'])), '/');

		// Telefon
		for($i=0; $i<strlen((string)$_POST['Suchtext']); $i++)
		{
			$c=substr((string)$_POST['Suchtext'], $i, 1);
			if(is_numeric($c))
			{
				if(strlen((string)$phone_pattern))
					$phone_pattern .= '\D*';
				$phone_pattern .= $c.'{1}';
			}
		}
	}
	if(strlen((string)$name_pattern))
		$name_pattern='/'.$name_pattern.'/i';
	if(strlen((string)$phone_pattern))
		$phone_pattern='/'.$phone_pattern.'/';
	else if(strlen((string)$name_pattern))
		$phone_pattern=$name_pattern;
//echo('$name_pattern: <b>'.fxHtmlEncode($name_pattern).'</b><br>');
//echo('$phone_pattern: <b>'.fxHtmlEncode($phone_pattern).'</b><br>');

	// Felder vor der Anzeige noch bearbeiten
	// -> Vorname zum Namen hinzufügen
	// -> Gleiche Namen nur einmal anzeigen
	if(fxIsArray($liste))
	{
		$merken				= '';
		$startzeile			= 0;
		$startzeile_merken	= 0;
		$anzeigen			= false;

		// Alle Kommunikationstypen ermitteln
		$ct=fxf_getReferenceValues('kommunikationstyp');

		foreach($liste as $zeile => $wert)
		{
			// Vorname zum Namen hinzufügen und komplett eliminieren
			if(!is_null($wert['vorname']) && strlen((string)$wert['vorname']))
				$liste[$zeile]['p_name']=$wert['p_name'].', '.$wert['vorname'];
			unset($liste[$zeile]['vorname']);

			// Prüfen, ob neuer Name
			if($liste[$zeile]['p_name'] != $merken)
			{
				// Alten Namen anzeigen?
				if($anzeigen)
				{
					$nadd=tl_add_listenwerte($neu);
					$startzeile += $nadd;
				}
				else
				{
					if($tl_debug)
						echo('<B><FONT COLOR="#880000">$anzeigen nicht gesetzt -> $startzeile rücksetzen auf gemerkten Wert</FONT></B><BR>');
					$startzeile=$startzeile_merken;
				}

				// Array für neuen Namen löschen
				$anzeigen=false;
				unset($neu);
				if($tl_debug)
					echo('Name "'.$liste[$zeile]['p_name'].'" ungleich "'.$merken.'"<BR>');
				$startzeile_merken=$startzeile;
				$merken=$liste[$zeile]['p_name'];
			}
			else
				$liste[$zeile]['p_name']='';

			// und erste Zeile eintragen
			$neu[$startzeile]=$liste[$zeile];
			if($tl_debug)
				fxDebug($neu, '$startzeile_merken='.$startzeile_merken.' -> neue $startzeile='.($startzeile+1));

			$lnk='';	// E-Mail, Homepage
			$img='';
			$neu_telefon=strtoupper((string)$neu[$startzeile]['telefonnummer']);
			// E-Mail: Typ, oder @-Zeichen
			if(($liste[$zeile]['typ'] == 2366) || (strpos($neu_telefon, '@') !== false))
			{
				$lnk='<a href="mailto:'.$neu[$startzeile]['telefonnummer'].'?subject=fx-project">';
				$img='msg';
			}
			// Homepage: HTTP, FTP oder WWW
			else if(($liste[$zeile]['typ'] == 2367) || (substr((string)$neu_telefon, 0,4) == 'HTTP') || (substr((string)$neu_telefon, 0,3) == 'FTP') || (strpos($neu_telefon, 'WWW.') !== false))
			{
				if(strpos($neu_telefon, ':') !== false)
					$lnk='<a href="'.$neu[$startzeile]['telefonnummer'].'" target="_blank">';
				else
					$lnk='<a href="https://'.$neu[$startzeile]['telefonnummer'].'" target="_blank">';
				$img='inv';
			}
			// Fax, Handy oder Telefon: Typ
			else if(($liste[$zeile]['typ'] == 1872) || ($liste[$zeile]['typ'] == 1873) || ($liste[$zeile]['typ'] == 2365))
			{
				$lnk='<a href="tel:'.$neu[$startzeile]['telefonnummer'].'">';
				$img='cont';
			}
			$neu[$startzeile]['link']=$lnk;
			$neu[$startzeile]['img']=$img;

			// Kein Suchtext mitgegeben, dann alles anzeigen
			if(!strlen((string)$name_pattern))
				$anzeigen=true;
			// Sonst prüfen, ob Suchtext gefunden wird
			else
			{
				if($tl_debug)
					echo('<B>Prüfen, ob Datensatz gültig -> anzeigen</B><BR>');

				// Name
				if(strlen((string)$neu[$startzeile]['p_name']))
				{
					$match=preg_match($name_pattern, $neu[$startzeile]['p_name']);
//fxDebug($match, $neu[$startzeile]['p_name'], 0);
					if($match)
					{
						$neu[$startzeile]['p_name']=preg_replace($name_pattern, $replace, $neu[$startzeile]['p_name']);
						$anzeigen=true;
					}
				}

				// Kommunikation
				if(strlen((string)$neu[$startzeile]['telefonnummer']))
				{
					$match=preg_match($phone_pattern, $neu[$startzeile]['telefonnummer']);
//fxDebug($match, $neu[$startzeile]['telefonnummer'], 0);
					if($match)
					{
						$neu[$startzeile]['telefonnummer']=preg_replace($phone_pattern, $replace, $neu[$startzeile]['telefonnummer']);
						$anzeigen=true;
					}
				}
			}

			$startzeile++;
		}

		// Letzten alten Namen anzeigen?
		if($anzeigen)
		{
			$nadd=tl_add_listenwerte($neu);
			fxf_maskColumnBorder(array('P_Name', 'Telefonnummer'), 1, -1, $maskenzaehler, $startzeile+$nadd, true);
		}
		else if($startzeile)
			fxf_maskColumnBorder(array('P_Name', 'Telefonnummer'), 1, -1, $maskenzaehler, $startzeile, true);

		if(!is_array($GLOBALS['fxptdata']['lvalues'][$maskenzaehler]))
			$GLOBALS['fxptdata']['lvalues'][$maskenzaehler]=true;
	}
	else
		$GLOBALS['fxptdata']['lvalues'][$maskenzaehler]=true;
}
else
	$maske406_nv=true;

////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $neu - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function tl_add_listenwerte($neu)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['tl_debug'])
		echo('<B><FONT COLOR="#008800">$anzeigen gesetzt -> Alten Namen anzeigen</FONT></B><BR>');
	$nadd=0;
	$naddchk=true;
	foreach($neu as $nzeile => $neuwert)
	{
		if($nzeile && $naddchk)
		{
			fxf_maskColumnBorder(array('P_Name', 'Telefonnummer'), 1, -1, $GLOBALS['maskenzaehler'], $nzeile, true);
			$nadd=1;
		}
		$naddchk=false;

		// Namenszeile gesondert behandeln
		if($neuwert['p_name'])
		{
			$bildchen=person_pic($neuwert['personen_id'], 18, $neuwert['geschlecht'], $GLOBALS['fxpglobals']['client'], 0);
//fxDebug($bildchen,'$bildchen: '.$neuwert['personen_id'], 0);
			$azusatz='?pid='.$neuwert['personen_id'].'&ptyp='.$neuwert['personentyp'];
			$ntr=0;
			if(($neuwert['personentyp'] == FXP_PT_CLIENT) && tr_rechte(166))
				$ntr=166;
			else if(($neuwert['personentyp'] == FXP_PT_CLIENT_CP) && tr_rechte(164))
				$ntr=164;
			else if(($neuwert['personentyp'] == FXP_PT_CUSTOMER) && tr_rechte(169))
				$ntr=169;
			else if(($neuwert['personentyp'] == FXP_PT_CUSTOMER_CP) && tr_rechte(170))
				$ntr=170;
			else if(($neuwert['personentyp'] == FXP_PT_PARTNER) && tr_rechte(173))
				$ntr=173;
			else if(($neuwert['personentyp'] == FXP_PT_PARTNER_CP) && tr_rechte(174))
				$ntr=174;
			else if(($neuwert['personentyp'] == FXP_PT_EMPLOYEE) && tr_rechte(167))
				$ntr=167;
			else if(($neuwert['personentyp'] == FXP_PT_CONTRACTOR) && tr_rechte(168))
				$ntr=168;
			else if(($neuwert['personentyp'] == FXP_PT_CONTACT) && tr_rechte(171))
				$ntr=171;
			else if(($neuwert['personentyp'] == FXP_PT_CONTACT_CP) && tr_rechte(172))
				$ntr=172;

			if($neuwert['inaktiv'])
				$neuwert['p_name']='<font class="grey">'.$neuwert['p_name'].'</font>';

			if($ntr)
			{
				if(!isset($GLOBALS['ltt_'.$ntr]))
					$GLOBALS['ltt_'.$ntr]='<b>'.meldung(1697,false).'</b>: '.trPath($ntr);	// Sprung zu
				$GLOBALS['p_name_cls']='^col_eaeaea|~';
				$neuwert['p_name']=$GLOBALS['p_name_cls'].fieldLinkImage('lnk'.($nzeile+$nadd), $bildchen, $azusatz, $GLOBALS['ltt_'.$ntr], $ntr, 'position:absolute;border:1px solid #aaaaaa;border-radius:4px;background:#ffffff;padding:1px;',18,18).'&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.$neuwert['p_name'];
			}
			else
			{
				$GLOBALS['p_name_cls']='^col_e8e8e8|~';
				$neuwert['p_name']=$GLOBALS['p_name_cls'].'<img src="'.$bildchen.'" style="position:absolute;width:18px;height:18px;border:1px solid #aaaaaa;border-radius:4px;background:#ffffff;padding:1px;">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<i>'.$neuwert['p_name'].'</i>';
			}
			if($neuwert['inaktiv'])
				$neuwert['p_name'] .= $GLOBALS['inakt'];
		}
		else
			$neuwert['p_name']=$GLOBALS['p_name_cls'];

		if($neuwert['img'] == 'inv')
			$neuwert['telefonnummer']='<span style="border-bottom:1px dotted #006b9f;">'.$neuwert['telefonnummer'].'</span>';

		if(strlen((string)$neuwert['img']))
			$img=$neuwert['img'];
		else
			$img='empty';
		$neuwert['telefonnummer']='<img src="'.$GLOBALS['gfxpath'].'c_'.$img.'.png" align="top">&nbsp;'.$neuwert['telefonnummer'];

		if($neuwert['link'])
			$neuwert['telefonnummer']=$neuwert['link'].$neuwert['telefonnummer'].'</a>';

		// Kommunikationstyp
		if($neuwert['typ'] && isset($GLOBALS['ct'][$neuwert['typ']]))
			$neuwert['telefonnummer']='<div class="grey" style="float:left;width:200px;">'.$GLOBALS['ct'][$neuwert['typ']].':&nbsp;</div><div style="float:left;">'.text_wrap($neuwert['telefonnummer'],true,90).'</div>';

		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['maskenzaehler']][$nzeile+$nadd]=$neuwert;
	}
	if($GLOBALS['tl_debug'])
		fxDebug($GLOBALS['fxptdata']['lvalues'][$GLOBALS['maskenzaehler']], 'maskenzaehler -- '.__LINE__);

	return $nadd;
}
?>