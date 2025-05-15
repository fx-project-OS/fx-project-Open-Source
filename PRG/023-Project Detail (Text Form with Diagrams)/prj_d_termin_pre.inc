<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_d_termin_pre.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2020-10-29                                                   //
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
 * PF 23: Project Detail (Text Form with Diagrams)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

enable_dummies();

// Texte
$std=meldung(484, true);	// Std.
$nstart='~<i class="s3 grey">['.str_replace(' ', '&nbsp;', fxStrToLower(meldung(1793, true))).']</i>';	// nicht gestartet
$nverf='~<i class="s3 grey">['.str_replace(' ', '&nbsp;', fxStrToLower(meldung(1132, true))).']</i>';	// nicht verfügbar
$nend='~<i class="s3 grey">['.fxStrToLower(meldung(1794, true)).']</i>';								// offen
$unbegr='~<i class="s3 grey">['.fxStrToLower(meldung(1795, true)).']</i>';								// unbegrenzt

// Minimum- und Maximumtermine ermitteln
if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) && $vEPID)
	$mmd=getMinMaxDates($vEPID);
else if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $vAPID)
	$mmd=getMinMaxDates($vAPID);
else
	$mmd=getMinMaxDates(0);
echo('<div id="dmmdates" style="display:none;">'.$mmd['minstart'].','.$mmd['maxstart'].','.$mmd['minend'].','.$mmd['maxend'].'</div>');

// Tooltip Minimum- und Maximumtermine
if(($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	$mzs=meldung(1105,true);	// Zeitspanne

	if(($mmd['minstart'] != '00000000') || ($mmd['maxstart'] != '99999999'))
	{
		$Soll_Beg_Dtm_tooltip='<b>'.$mzs.':</b><br>';
		if($mmd['minstart'] != '00000000')
			$Soll_Beg_Dtm_tooltip .= get_entry($mmd['minstart'],'datum');
		else
			$Soll_Beg_Dtm_tooltip .= '?';
		$Soll_Beg_Dtm_tooltip .= ' - ';
		if($mmd['maxstart'] != '999999')
			$Soll_Beg_Dtm_tooltip .= get_entry($mmd['maxstart'],'datum');
		else
			$Soll_Beg_Dtm_tooltip .= '?';
	}
	if(($mmd['minend'] != '00000000') || ($mmd['maxend'] != '99999999'))
	{
		$Soll_End_Dtm_tooltip='<b>'.$mzs.':</b><br>';
		if($mmd['minend'] != '00000000')
			$Soll_End_Dtm_tooltip .= get_entry($mmd['minend'],'datum');
		else
			$Soll_End_Dtm_tooltip .= '?';
		$Soll_End_Dtm_tooltip .= ' - ';
		if($mmd['maxend'] != '999999')
			$Soll_End_Dtm_tooltip .= get_entry($mmd['maxend'],'datum');
		else
			$Soll_End_Dtm_tooltip .= '?';
	}
}

// Variablen
$fgrad=0.0;
$jetzt=substr((string)fxNow(), 0, 8).'000000';

// Felder ermitteln
if($err)
{
	$vSBD=validate_entry($_POST['Soll_Beg_Dtm'], $verr, 'datum');
	$vSED=validate_entry($_POST['Soll_End_Dtm'], $verr, 'datum');

	$vDL=validate_entry($_POST['Deadline'], $verr, 'datum');

	$vAS=effortUnit2Sec(validate_entry($_POST['Aufwand_Soll'], $verr, 'dezimal'));
	$vMZA=effortUnit2Sec(validate_entry($_POST['Max_Zeit_Aufw'], $verr, 'dezimal'));
}
else
{
	if(fxIsArray($pinfo))
	{
		if($vAPID)
		{
			$vSBD=substr((string)$pinfo[$vAPID]['data']['soll_beginn'], 0, 8).'000000';
			$vSED=substr((string)$pinfo[$vAPID]['data']['soll_ende'], 0, 8).'240000';

			$vAS=effortSec2Unit($pinfo[$vAPID]['data']['aufwand_soll']);
			$vMZA=effortSec2Unit($pinfo[$vAPID]['data']['max_zeit_aufw']);

			if(!is_null($pinfo[$vAPID]['data']['deadline']) && strlen((string)$pinfo[$vAPID]['data']['deadline']))
				$vDL=substr((string)$pinfo[$vAPID]['data']['deadline'], 0, 8).'240000';
		}
		else if($vEPID)
		{
			$vSBD=substr((string)$pinfo[$vEPID]['data']['soll_beginn'], 0, 8).'000000';
			$vSED=substr((string)$pinfo[$vEPID]['data']['soll_ende'], 0, 8).'240000';

			if(!is_null($pinfo[$vEPID]['data']['deadline']) && strlen((string)$pinfo[$vEPID]['data']['deadline']))
				$vDL=substr((string)$pinfo[$vEPID]['data']['deadline'], 0, 8).'240000';

			// Wird eine Aufgabe direkt unter ein Mutterprojekt gehängt dann zusätzliche Felder vorbelegen
			if(!$eltex && ($pinfo[$vEPID]['data']['projektart'] == FXP_PRJ_MAIN) && ($vPART == FXP_PRJ_TASK))
			{
				$vNP=trim((string)$pinfo[$vEPID]['data']['name_projekt']).'*';
				$vBESCH=$pinfo[$vEPID]['data']['beschreibung'];

				$vAS=effortSec2Unit($pinfo[$vEPID]['data']['aufwand_soll']);
				$vMZA=effortSec2Unit($pinfo[$vEPID]['data']['max_zeit_aufw']);
			}
		}
	}
}

if(fxIsArray($pinfo))
{
	if($vAPID)
	{
		$vIBD=$pinfo[$vAPID]['data']['ist_beginn'];
		$vIED=$pinfo[$vAPID]['data']['ist_ende'];
		$vAI=effortSec2Unit($pinfo[$vAPID]['aufwand_ist']);
		$vSV=effortSec2Unit($pinfo[$vAPID]['ist_verzoeg']+$pinfo[$vAPID]['soll_verzoeg']);

		$fgrad=$pinfo[$vAPID]['rfgrad'];

		// Abrechenbarer Ist-Aufwand
		$psql  = "SELECT p.projekt_id FROM projekte p INNER JOIN budget_summe b ON (b.projekt_id=p.projekt_id AND b.mandanten_id=p.mandanten_id) WHERE p.vorgangsnummer LIKE '".$pinfo[$vAPID]['data']['vorgangsnummer']."%' AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.abrechnungsart<>".FXP_INV_NOT;
		$asql  = "SELECT SUM(zeitaufwand) AS ai FROM zeitdaten";
		$asql .= " WHERE projekt_id IN (".$psql.") AND mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id>0 AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.")";
		$asql .= " AND (nicht_abrechenbar IS NULL OR nicht_abrechenbar=0)";
		$vAA=effortSec2Unit((int)db_value($asql));
//fxDebug($vAA,$asql, 0);
	}

	if($vEPID)
	{
		$vSBE=$pinfo[$vEPID]['data']['soll_beginn'];
		$vSEE=$pinfo[$vEPID]['data']['soll_ende'];
	}
	else
	{
		$vSBE=$nverf;
		$vSEE=$nverf;
	}
}

$maskenfeldwerte['bis']='~-';

// ...Beginn Vorgänger (SollBeginnElt) + Ende Vorgänger (SollEndeElt)
if($vEPID)
{
	$maskenfeldwerte['sollbeginnelt']='~'.get_entry($vSBE,'date').'&nbsp;&nbsp;&nbsp;&nbsp;';
	$maskenfeldwerte['von']='~-';
	$maskenfeldwerte['sollendeelt']='~'.get_entry($vSEE,'date').'&nbsp;&nbsp;&nbsp;&nbsp;';
}
else
{
	$SollBeginnElt_nv=true;
	$Von_nv=true;
	$SollEndeElt_nv=true;
}

// ...Geplantes Startdatum (Soll_Beg_Dtm)
if(!is_null($vSBD) && strlen((string)$vSBD))
	$maskenfeldwerte['soll_beg_dtm']=$vSBD;

// ...Geplantes Enddatum (Soll_End_Dtm)
if(!is_null($vSED) && strlen((string)$vSED))
	$maskenfeldwerte['soll_end_dtm']=$vSED;

// ...Deadline (Deadline)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
{
	if(!is_null($vDL) && strlen((string)$vDL))
		$maskenfeldwerte['deadline']='~&nbsp;<b class="red">'.get_entry($vDL, 'datum').'</b>&nbsp;';
	else
		$maskenfeldwerte['deadline']=$unbegr;
}
else if(!is_null($vDL) && strlen((string)$vDL))
	$maskenfeldwerte['deadline']=$vDL;
else
	$maskenfeldwerte['deadline']='';

// ...Aufwand Soll (Aufwand_Soll)
if(!fxIsArray($vAS))
	$vAS=effortSec2Unit(0);
$maskenfeldwerte['aufwand_soll']=$vAS['val2'];
$maskenfeldwerte['zeiteinheit_aufw_s']=$vAS['unit1'];
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	$Aufwand_Soll_sf=2;
else if($vPART != FXP_PRJ_TASK)
	$Aufwand_Soll_sf=2;
$Zeiteinheit_Aufw_S_sf=2;

// ...max. Zeitaufwand (Max_Zeit_Aufw)
if(!fxIsArray($vMZA))
	$vMZA=effortSec2Unit(0);
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
	$Max_Zeit_Aufw_sf=2;
else if($vPART != FXP_PRJ_TASK)
	$Max_Zeit_Aufw_sf=2;
if(isset($Max_Zeit_Aufw_sf) && ($Max_Zeit_Aufw_sf == 2))
{
	if($vMZA['sec'] > 0)
		$maskenfeldwerte['max_zeit_aufw']='~<b class="red">'.$vMZA['val2'].'</b>';
	else
		$maskenfeldwerte['max_zeit_aufw']=$unbegr;
}
else if($vMZA['sec'] > 0)
	$maskenfeldwerte['max_zeit_aufw']=$vMZA['val2'];
$maskenfeldwerte['zeiteinheit_max_zeitaufw']=$vMZA['unit1'];
$Zeiteinheit_max_Zeitaufw_sf=2;

// ...voraus. Verzögerung (Soll_Verzoeg)
if(!fxIsArray($vSV))
	$vSV=effortSec2Unit(0);
$maskenfeldwerte['soll_verzoeg']='~'.$vSV['val2'];
$maskenfeldwerte['zeiteinheit_soll_verz']=$vSV['unit1'];
$Soll_Verzoeg_sf=2;
$Zeiteinheit_Soll_Verz_sf=2;

// ...Aufwand Ist (Aufwand_Ist)
if(!fxIsArray($vAI))
	$vAI=effortSec2Unit(0);
$maskenfeldwerte['aufwand_ist']='~<b>'.$vAI['val2'].'</b>';
$maskenfeldwerte['zeiteinheit_aufw_i']='~'.$vAI['unit2'];
$Aufwand_Ist_sf=2;
$Zeiteinheit_Aufw_I_sf=2;
// ...abrechenbar (nv_zahl1)
if(!fxIsArray($vAA))
	$vAA=effortSec2Unit(0);
$maskenfeldwerte['nv_zahl1']='~<span id="eff_aei" class="green">'.$vAA['val2'].'</span>';
$maskenfeldwerte['nv_ganzzahl1']='~<span id="unit_eff_aei" class="green">'.$vAA['unit2'].'</span>';
$nv_zahl1_sf=2;
$nv_ganzzahl1_sf=2;
// ...n. abrechenbar (nv_zahl2)
$vAN=effortSec2Unit($vAI['sec']-$vAA['sec']);
$maskenfeldwerte['nv_zahl2']='~<span id="eff_aen" class="red">'.$vAN['val2'].'</span>';
$maskenfeldwerte['nv_ganzzahl2']='~<span id="unit_eff_aen" class="red">'.$vAN['unit2'].'</span>';
$nv_zahl2_sf=2;
$nv_ganzzahl2_sf=2;

// ...Tatsächl. Startdatum (Ist_Beg_Dtm) + Tatsächl. Enddatum (Ist_End_Dtm)
if($vIBD)
{
	$vIBD=substr((string)$vIBD, 0, 8).'000000';
	$maskenfeldwerte['ist_beg_dtm']='~'.get_entry($vIBD,'date').'&nbsp;&nbsp;&nbsp;&nbsp;';
}
else
	$maskenfeldwerte['ist_beg_dtm']=$nstart.'&nbsp;&nbsp;&nbsp;&nbsp;';
if($vIED)
{
	$vIED=substr((string)$vIED, 0, 8).'240000';
	$maskenfeldwerte['ist_end_dtm']='~'.get_entry($vIED,'date').'&nbsp;&nbsp;&nbsp;&nbsp;';
}
else
	$maskenfeldwerte['ist_end_dtm']=$nend.'&nbsp;&nbsp;&nbsp;&nbsp;';

if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	// ...Verlauf-Zeit (Verlauf_Zeit)
	// Vorgang wurde begonnen...
	if($vIBD && ($vIBD <= $jetzt))
	{
		// Differenz Soll-/Ist-Beginn
		$delay=time_diff($vSBD, $vIBD, 'd');
		if($delay <= 1.0)
			$tt=meldung(2233, false, get_entry(-$delay, 'ganzzahl'));	// Dieser Vorgang begann $var1 Tag(e) früher als geplant.
		else if($delay >= 1.0)
			$tt=meldung(2235, false, get_entry($delay, 'ganzzahl'));	// Dieser Vorgang begann $var1 Tag(e) später als geplant.
		else 
			$tt=meldung(2234, false);									// Dieser Vorgang begann zum geplanten Zeitpunkt.
		$tt="\n".$tt;

		// Kein Ist-Ende => mit "heute" rechnen
		if($vIED)
			$ieg=substr((string)$vIED, 0, 8).'240000';
		else
			$ieg=substr((string)$jetzt, 0, 8).'240000';

		// Wie viele Tage wird an dem Projekt bereits gearbeitet
		$dwork=1 + time_diff($vIBD, $ieg, 'd');
		// Wie lange dauert der Vorgang planungsgemäß
		$dspan=1 + time_diff($vSBD, $vSED, 'd');
		// Wieviel Prozent der eingeplanten Tage entspricht das
		$ptime=0.0;
		if($dspan > 0)
			$ptime=$dwork / $dspan;
		if($ptime <= $fgrad)
			$barcol='green';
		else if($ptime >= ($fgrad + 0.10))
			$barcol='red';
		else
			$barcol='yellow';
		if($ptime > 1.0)
			$ptime=1.0;

		// Farbbalken => alternativer Text "Es wurden $var1% der Zeit benötigt."
		$gpercent=$ptime*100.0;
		$tt=meldung(2231, false, get_entry($gpercent, 'dezimal')).$tt;
	}
	// ...bzw. noch nicht begonnen
	else
	{
		// Differenz Jetzt/Soll-Beginn
		$diff=time_diff($vSBD, $jetzt, 'd');
		if($diff < -1.0)
		{
			$tt=meldung(1796, false, get_entry(-$diff, 'ganzzahl'));	// Dieser Vorgang beginnt voraussichtlich in $var1 Tag(en).
			$barcol='green';
		}
		else if($diff < 0.0)
		{
			$tt=meldung(1797, false);									// Dieser Vorgang beginnt voraussichtlich morgen.
			$barcol='green';
		}
		else if(!$diff)
		{
			$tt=meldung(1798, false);									// Dieser Vorgang sollte heute beginnen.
			$barcol='yellow';
		}
		else
		{
			$tt=meldung(1799, false, get_entry($diff, 'ganzzahl'));	// Dieser Vorgang sollte eigentlich vor $var1 Tag(en) begonnen worden sein.
			$barcol='red';
		}

		if($diff < 0.0)
		{
			// Wie viele Tage bis zum voraussichtlichen Enddatum
			$dend=1 + time_diff($jetzt, $vSED, 'd');
			// Wie lange dauert der Vorgang planungsgemäß
			$dspan=1 + time_diff($vSBD, $vSED, 'd');
			// Wieviel Prozent der eingeplanten Tage entspricht das
			$ptime=$dspan / $dend;
			$gpercent=-$ptime*100.0;
		}
		else
			$gpercent=100.0;
	}
	$maskenfeldwerte['verlauf_zeit']='~'.bar_horizontal($gpercent, $tt, $barcol);

	// ...Verlauf-Aufwand (Verlauf_Aufwand)
	if($vAS['sec'] > 0)
		$gpercent=($vAI['sec'] / ($vAS['sec'] + $vSV['sec']))*100.0;
	else
		$gpercent=0.0;
	$tt=meldung(2229, false, get_entry($gpercent, 'dezimal'));
	$bild='';
	if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) && isset($GLOBALS['_ext_chartdir']) && !is_null($GLOBALS['_ext_chartdir']) && strlen((string)$GLOBALS['_ext_chartdir']))
	{
		$heute=substr((string)fxNow(), 0, 8);
		// Startdatum ermitteln
		$sbdat=substr((string)$pinfo[$vAPID]['data']['soll_beginn'], 0, 8);
		$sdate=$sbdat;
		$ibdat='';
		if($pinfo[$vAPID]['data']['ist_beginn'])
		{
			$ibdat=substr((string)$pinfo[$vAPID]['data']['ist_beginn'], 0, 8);
			$sdate=min($sdate, $ibdat);
		}
		// Enddatum ermitteln
		$sedat=substr((string)$pinfo[$vAPID]['data']['soll_ende'], 0, 8);
		$edate=$sedat;
		$iedat='';
		if($pinfo[$vAPID]['data']['ist_ende'])
		{
			$iedat=substr((string)$pinfo[$vAPID]['data']['ist_ende'], 0, 8);
			$edate=max($edate, $iedat);
		}
		// Start- und Enddatum korrigieren, falls zu grosse Zeitspanne
		$seda=get_datearray_range($sdate, $edate, $pinfo[$vAPID]['data']['projekt_status']);
		$sdate=$seda[0];
		$edate=$seda[1];
		$zsdate=time_add($sdate, -1, 'D');
		$zedate=time_add($edate, 1, 'D');

		// Tagesdifferenz
		$tarray=array();
		$darray=array();
		$maxtag=0;
		$cim=(int)(time_diff($zsdate, $zedate, 'd')/30);

		$motage=array(1=>31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31);
		$sy=(int)substr((string)$zsdate, 0, 4);
		$sm=(int)substr((string)$zsdate, 4, 2);
		$sd=(int)substr((string)$zsdate, 6, 2);

		$lj=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $sy));
		if($lj)
			$motage[2]=29;
		else
			$motage[2]=28;

		// Tage
		$adate=$zsdate;
		while($adate <= $zedate)
		{
			$tarray[$adate]	= $maxtag;

			$dt='';
			if($adate == $heute)
				$dt=get_entry($adate, 'datum');
			else if(($adate == $sdate) || ($adate == $edate) || ($adate == $sbdat) || ($adate == $sedat) || ($adate == $ibdat) || ($adate == $iedat))
			{
				// Prüfen, ob Datum angezeigt werden soll
				$cs=$maxtag;
				for($ci=0; $ci<$cim; $ci++)
				{
					$cc=$cs-$ci;
					if(($cc >= 0) && isset($darray['lab'][$cc]) && !is_null($darray['lab'][$cc]) && strlen((string)$darray['lab'][$cc]))
					{
						$cs=-1;
						break;
					}
				}
				if($cs >= 0)
					$dt=get_entry($adate, 'datum');
			}
			$darray['lab'][$maxtag]=$dt;

			$darray[0][$maxtag]=0.0;
			$darray[1][$maxtag]=NoValue;
			$darray[2][$maxtag]=NoValue;

			$sd++;
			if($sd > $motage[$sm])
			{
				$sd=1;
				$sm++;
				if($sm > 12)
				{
					$sm=1;
					$sy++;
					$lj=fx_date('L', fx_mktime(12, 0, 0, 1, 1, $sy));
					if($lj)
						$motage[2]=29;
					else
						$motage[2]=28;
				}
			}

			$adate=substr('0000'.$sy, -4).substr('00'.$sm, -2).substr('00'.$sd, -2);

			$maxtag++;
		}

		// Ist-Verlauf
		$sql  = "SELECT zeitaufwand, ist_beginn FROM zeitdaten WHERE projekt_id IN (".$infvn.") AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (zeitaufwand IS NOT NULL AND zeitaufwand>0)";
		$sql .= " AND (ist_beginn>='".$sdate."000000' AND ist_beginn<='".$edate."240000') AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY ist_beginn";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			// Evtl. Startsumme ermitteln und Werte vorbelegen
			$sql  = "SELECT SUM(zeitaufwand) AS startaufwand FROM zeitdaten WHERE projekt_id IN (".$infvn.")";
			$sql .= " AND zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (zeitaufwand IS NOT NULL AND zeitaufwand>0) AND ist_beginn<'".$sdate."000000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$sb=(int)db_value($sql);
//fxDebug($sb, $sql, 3);
			if($sb > 0)
			{
				$sb=$sb/3600.0;
				for($cnt=1; $cnt<$maxtag; $cnt++)
					$darray[0][$cnt]=$sb;
			}

			foreach($tmp as $t)
			{
				$ib=$t['zeitaufwand']/3600.0;
				$dt	= max($sdate, substr((string)$t['ist_beginn'], 0, 8));
				for($cnt=$tarray[$dt]+1; $cnt<$maxtag; $cnt++)
					$darray[0][$cnt] += $ib;
			}
		}
		$darray[0][0]=NoValue;
		$darray[0][$maxtag-1]=NoValue;

		// Soll-Verlauf
		$igb=$pinfo[$vAPID]['data']['aufwand_soll']/3600.0;
		$sbdiff=time_diff($zsdate, $sbdat, 'd');
		$stage=1 + time_diff($sbdat, $sedat, 'd');
		$sdiff=$igb/$stage;
		for($cnt=$sbdiff; $cnt<$sbdiff+$stage+1; $cnt++)
		{
			if($cnt && ($cnt < $maxtag))
			{
				$iv=min($igb, ($cnt-$sbdiff)*$sdiff);
				$darray[1][$cnt]=$iv;
			}
		}
		$darray[1][$maxtag-1]=NoValue;

		// Soll-Verlauf (H)
		$sql="SELECT aufwand_soll, zeitstempel FROM projekte_history WHERE projekt_id=".$vAPID;
		if($vPART < FXP_PRJ_TASK)
			$sql .= " AND aktion=3";
		$sql .= " AND aufwand_soll>0 AND zeitstempel<='".$edate."240000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY zeitstempel";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$sb=$t['aufwand_soll']/3600.0;
				$dt=max($sdate, substr((string)$t['zeitstempel'], 0, 8));

				// Prüfen, ob Datum angezeigt werden soll
				$cs=$tarray[$dt];
				for($ci=0; $ci<$cim; $ci++)
				{
					$cc=$cs-$ci;
					if(($cc >= 0) && !is_null($darray['lab'][$cc]) && strlen((string)$darray['lab'][$cc]))
					{
						$cs=-1;
						break;
					}
					$cc=$cs+$ci;
					if(($cc < $maxtag-1) && !is_null($darray['lab'][$cc]) && strlen((string)$darray['lab'][$cc]))
					{
						$cs=-1;
						break;
					}
				}
				if($cs > 0)
					$darray['lab'][$cs]=get_entry($dt, 'datum');

				for($cnt=$tarray[$dt]; $cnt<$maxtag; $cnt++)
					$darray[2][$cnt]=$sb;
			}
		}
		$darray[2][$maxtag-1]=NoValue;
//fxDebug($darray, '<b>Startdatum:</b> '.$sdate.', <b>Enddatum:</b> '.$edate, 3);

		$darray['col']=array(0=>0x60009f6b, 0xa0006b9f, 0xc0aaaaaa);
		$darray['leg']=array(0=>get_text(298, '20'), get_text(297, '20'), get_text(297, '20').' (H)');	// Aufwand Ist + Aufwand Soll + Aufwand Soll (H)

		// Hintergrundfarbe
		$bgcol=0xf0f0f0;

		$bild=bar_chartdir($darray, FXP_CT_AREA, 764, 192, 7, $tt, 'h', '', $bgcol);
	}
	if(!strlen((string)$bild))
		$bild=bar_horizontal($gpercent, $tt);
	$maskenfeldwerte['verlauf_aufwand']='~<div style="height:8px;overflow:hidden;">&nbsp;</div>'.$bild;
}
else
{
	$hr5_nv=true;
	$Verlauf_Zeit_nv=true;
	$Verlauf_Aufwand_nv=true;
}
?>