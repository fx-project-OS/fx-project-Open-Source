<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_e_budget_pre.inc                                         //
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

// Felder ermitteln
if($err)
{
	$vBRPI=validate_entry($_POST['budgetrahmen_pers_int'], $verr, 'dezimal');
	$vBRPE=validate_entry($_POST['budgetrahmen_pers_ext'], $verr, 'dezimal');
	$vBRMI=validate_entry($_POST['budgetrahmen_mat_int'], $verr, 'dezimal');
	$vBRME=validate_entry($_POST['budgetrahmen_mat_ext'], $verr, 'dezimal');
	$vBTYP=$_POST['budgettyp'];
	$vPABR=$_POST['Abrechnungsart'];
	$vMABR=$_POST['Abrechnungsart_Mat'];

	$vBPIG=validate_entry($_POST['budget_pers_int_gepl'], $verr, 'dezimal');
	$vBMIG=validate_entry($_POST['budget_mat_int_gepl'], $verr, 'dezimal');
	$vBPEG=validate_entry($_POST['budget_pers_ext_gepl'], $verr, 'dezimal');
	$vBMEG=validate_entry($_POST['budget_mat_ext_gepl'], $verr, 'dezimal');
}
else if(fxIsArray($pinfo))
{
	if($vAPID)
	{
		$vBTYP=$pinfo[$vAPID]['data']['budgettyp'];
		$vPABR=$pinfo[$vAPID]['data']['abrechnungsart'];
		$vMABR=$pinfo[$vAPID]['data']['abrechnungsart_mat'];
		$vBRPI=$pinfo[$vAPID]['data']['budgetrahmen_pers_int'];
		$vBRPE=$pinfo[$vAPID]['data']['budgetrahmen_pers_ext'];
		$vBRMI=$pinfo[$vAPID]['data']['budgetrahmen_mat_int'];
		$vBRME=$pinfo[$vAPID]['data']['budgetrahmen_mat_ext'];
		if(($vPART >= FXP_PRJ_TASK) && $vEPID)
		{
			$vBRPI=$pinfo[$vEPID]['data']['budgetrahmen_pers_int'];
			$vBRPE=$pinfo[$vEPID]['data']['budgetrahmen_pers_ext'];
			$vBRMI=$pinfo[$vEPID]['data']['budgetrahmen_mat_int'];
			$vBRME=$pinfo[$vEPID]['data']['budgetrahmen_mat_ext'];
		}

		$vBPIG=$pinfo[$vAPID]['data']['budget_pers_int_gepl'];
		$vBMIG=$pinfo[$vAPID]['data']['budget_mat_int_gepl'];
		$vBPEG=$pinfo[$vAPID]['data']['budget_pers_ext_gepl'];
		$vBMEG=$pinfo[$vAPID]['data']['budget_mat_ext_gepl'];
	}
	else if($vEPID)
	{
		$vBTYP=$pinfo[$vEPID]['data']['budgettyp'];
		$vPABR=$pinfo[$vEPID]['data']['abrechnungsart'];
		$vMABR=$pinfo[$vEPID]['data']['abrechnungsart_mat'];
		if($vPART >= FXP_PRJ_TASK)
		{
			$vBRPI=$pinfo[$vEPID]['data']['budgetrahmen_pers_int'];
			$vBRPE=$pinfo[$vEPID]['data']['budgetrahmen_pers_ext'];
			$vBRMI=$pinfo[$vEPID]['data']['budgetrahmen_mat_int'];
			$vBRME=$pinfo[$vEPID]['data']['budgetrahmen_mat_ext'];
		}

		// Wird eine Aufgabe direkt unter ein Mutterprojekt gehängt dann zusätzliche Felder vorbelegen
		if(!$eltex && ($pinfo[$vEPID]['data']['projektart'] == FXP_PRJ_MAIN) && ($vPART == FXP_PRJ_TASK))
		{
			$vBPIG=$pinfo[$vEPID]['data']['budget_pers_int_gepl'];
			$vBMIG=$pinfo[$vEPID]['data']['budget_mat_int_gepl'];
			$vBPEG=$pinfo[$vEPID]['data']['budget_pers_ext_gepl'];
			$vBMEG=$pinfo[$vEPID]['data']['budget_mat_ext_gepl'];
		}
	}
}

// ...Int. Personenbudget - Rahmen Geplant (budgetrahmen_pers_int)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || ($vPART == FXP_PRJ_TASK))
	$budgetrahmen_pers_int_sf=2;
if(!$vBRPI)
	$vBRPI=0.0;
$maskenfeldwerte['budgetrahmen_pers_int']=$vBRPI;
// ...Int. Personenbudget - Rahmen Rest (budgetrahmen_pers_int_rest)
$maskenfeldwerte['budgetrahmen_pers_int_rest']=$budgetrahmen['pers_int_rest'];

// ...Int. Materialbudget - Rahmen Geplant (budgetrahmen_mat_int)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || ($vPART == FXP_PRJ_TASK))
	$budgetrahmen_mat_int_sf=2;
if(!$vBRMI)
	$vBRMI=0.0;
$maskenfeldwerte['budgetrahmen_mat_int']=$vBRMI;
// ...Int. Materialbudget - Rahmen Rest (budgetrahmen_mat_int_rest)
$maskenfeldwerte['budgetrahmen_mat_int_rest']=$budgetrahmen['mat_int_rest'];

// ...Ext. Personenbudget - Rahmen Geplant (budgetrahmen_pers_ext)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || ($vPART == FXP_PRJ_TASK))
	$budgetrahmen_pers_ext_sf=2;
if(!$vBRPE)
	$vBRPE=0.0;
$maskenfeldwerte['budgetrahmen_pers_ext']=$vBRPE;
// ...Ext. Personenbudget - Rahmen Rest (budgetrahmen_pers_ext_rest)
$maskenfeldwerte['budgetrahmen_pers_ext_rest']=$budgetrahmen['pers_ext_rest'];

// ...Ext. Materialbudget - Rahmen Geplant (budgetrahmen_mat_ext)
if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || ($vPART == FXP_PRJ_TASK))
	$budgetrahmen_mat_ext_sf=2;
if(!$vBRME)
	$vBRME=0.0;
$maskenfeldwerte['budgetrahmen_mat_ext']=$vBRME;
// ...Ext. Materialbudget - Rahmen Rest (budgetrahmen_mat_ext_rest)
$maskenfeldwerte['budgetrahmen_mat_ext_rest']=$budgetrahmen['mat_ext_rest'];

// ...Budgettyp (budgettyp)
if(!$vBTYP)
	$vBTYP=702;	// Standardbudget
$budgettyp_ds=0;
$maskenfeldwerte['budgettyp']=$vBTYP;

// ...Abrechnungsart (Personen) (Abrechnungsart)
if(!$vPABR)
	$vPABR=$GLOBALS['fxpglobals']['settings']['def_abrechnart'];
if(!$vPABR || ($vPABR == FXP_INV_MAT_HU))
	$vPABR=FXP_INV_HOUR;	// Stundenbasis
$Abrechnungsart_ds=0;
$maskenfeldwerte['abrechnungsart']=$vPABR;

// ...Abrechnungsart (Material) (Abrechnungsart_Mat)
if(!$vMABR)
	$vMABR=$vPABR;
if($vMABR == FXP_INV_HOUR)
	$vMABR=FXP_INV_MAT_HU;	// Stunden- oder Mengenbasis
$Abrechnungsart_Mat_ds=0;
$maskenfeldwerte['abrechnungsart_mat']=$vMABR;

$iea=array('int'=>array('pers', 'mat'), 'ext'=>array('pers', 'mat'));
// ...Internes + Externes Budget
foreach($iea as $ie => $pma)
{
	// ...Personen- + Materialbudget
	${'valsum_'.$ie.'_gepl'}	= 0.0;
	${'valsum_'.$ie.'_gen'}		= 0.0;
	${'valsum_'.$ie.'_akt'}		= 0.0;
	${'valsum_'.$ie.'_verpl'}	= 0.0;
	${'valsum_'.$ie.'_verbr'}	= 0.0;
	${'valsum_'.$ie.'_rest'}	= 0.0;
	foreach($pma as $pm)
	{
		//...geplant
		if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE) || ($vPART != FXP_PRJ_TASK))
			${'budget_'.$pm.'_'.$ie.'_gepl_sf'}=2;
		${'val'.$pm.'_gepl'}=${'vB'.strtoupper(substr((string)$pm, 0, 1)).strtoupper(substr((string)$ie, 0, 1)).'G'};
		if(!${'val'.$pm.'_gepl'})
			${'val'.$pm.'_gepl'}=0.0;
		$maskenfeldwerte['budget_'.$pm.'_'.$ie.'_gepl']=${'val'.$pm.'_gepl'};
		${'valsum_'.$ie.'_gepl'} += ${'val'.$pm.'_gepl'};
		//...genehmigt
		if(is_array($pinfo) && $vAPID)
			${'val'.$pm.'_gen'}=$pinfo[$vAPID]['data']['budget_'.$pm.'_'.$ie.'_gen'];
		if(!${'val'.$pm.'_gen'})
			${'val'.$pm.'_gen'}=0.0;
		$maskenfeldwerte['budget_'.$pm.'_'.$ie.'_gen']=${'val'.$pm.'_gen'};
		${'valsum_'.$ie.'_gen'} += ${'val'.$pm.'_gen'};
		//...aktiviert
		if(is_array($pinfo) && $vAPID)
			${'val'.$pm.'_akt'}=$pinfo[$vAPID]['data']['budget_'.$pm.'_'.$ie.'_akt'];
		if(!${'val'.$pm.'_akt'})
			${'val'.$pm.'_akt'}=0.0;
		$maskenfeldwerte['budget_'.$pm.'_'.$ie.'_akt']=${'val'.$pm.'_akt'};
		${'valsum_'.$ie.'_akt'} += ${'val'.$pm.'_akt'};
		//...verplant
		if(is_array($pinfo) && $vAPID)
			${'val'.$pm.'_verpl'}=$pinfo[$vAPID]['data']['budget_'.$pm.'_'.$ie.'_verpl'];
		if(!${'val'.$pm.'_verpl'})
			${'val'.$pm.'_verpl'}=0.0;
		$maskenfeldwerte['budget_'.$pm.'_'.$ie.'_verpl']=${'val'.$pm.'_verpl'};
		${'valsum_'.$ie.'_verpl'} += ${'val'.$pm.'_verpl'};
		//...verbraucht
		if(is_array($pinfo) && $vAPID)
			${'val'.$pm.'_verbr'}=$pinfo[$vAPID]['data']['budget_'.$pm.'_'.$ie.'_verbr'];
		if(!${'val'.$pm.'_verbr'})
			${'val'.$pm.'_verbr'}=0.0;
		$maskenfeldwerte['budget_'.$pm.'_'.$ie.'_verbr']=${'val'.$pm.'_verbr'};
		${'valsum_'.$ie.'_verbr'} += ${'val'.$pm.'_verbr'};
		//...Rest
		${'val'.$pm.'_rest'}=${'val'.$pm.'_gen'}-${'val'.$pm.'_verpl'}-${'val'.$pm.'_verbr'};
		$maskenfeldwerte['budget_'.$pm.'_'.$ie.'_rest']=${'val'.$pm.'_rest'};
		${'valsum_'.$ie.'_rest'} += ${'val'.$pm.'_rest'};
	}
	$maskenfeldwerte['budget_ges_'.$ie.'_gepl']		= ${'valsum_'.$ie.'_gepl'};
	$maskenfeldwerte['budget_ges_'.$ie.'_gen']		= ${'valsum_'.$ie.'_gen'};
	$maskenfeldwerte['budget_ges_'.$ie.'_akt']		= ${'valsum_'.$ie.'_akt'};
	$maskenfeldwerte['budget_ges_'.$ie.'_verpl']	= ${'valsum_'.$ie.'_verpl'};
	$maskenfeldwerte['budget_ges_'.$ie.'_verbr']	= ${'valsum_'.$ie.'_verbr'};
	$maskenfeldwerte['budget_ges_'.$ie.'_rest']		= ${'valsum_'.$ie.'_rest'};
}

if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE))
{
	// ...Verlauf-Kosten (Verlauf_Kosten)
	if($valsum_int_gepl > 0.0)
		$gpercent=(($valsum_int_verpl+$valsum_int_verbr) / $valsum_int_gepl)*100.0;
	else
		$gpercent=$pinfo[$vAPID]['angrad'] * 100.0;
	$tt=meldung(2230, false, get_entry($gpercent, 'dezimal'));

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
			$tarray[$adate]=$maxtag;

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

		// Int. verbr. Gesamtbudget
		$sql="SELECT (budget_pers_int_verbr+budget_mat_int_verbr) AS budget_ges_int_verbr, zeitstempel FROM budget_summe_history WHERE projekt_id=".$vAPID;
		if($vPART < FXP_PRJ_TASK)
			$sql .= " AND aktion=".FXP_CHANGE;
		$sql .= " AND zeitstempel<='".$edate."240000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY zeitstempel, autoid";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$dt=max($sdate, substr((string)$t['zeitstempel'], 0, 8));

				for($cnt=$tarray[$dt]+1; $cnt<$maxtag; $cnt++)
					$darray[0][$cnt]=$t['budget_ges_int_verbr'];
			}
		}
		$darray[0][0]=NoValue;
		$darray[0][$maxtag-1]=NoValue;

		// Int. gepl. Gesamtbudget (H)
		$sql="SELECT (budget_pers_int_gepl+budget_mat_int_gepl) AS budget_ges_int_gepl, zeitstempel FROM budget_summe_history WHERE projekt_id=".$vAPID;
		if($vPART < FXP_PRJ_TASK)
			$sql .= " AND aktion=".FXP_CHANGE;
		$sql .= " AND zeitstempel<='".$edate."240000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY zeitstempel, autoid";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$dt=max($sdate, substr((string)$t['zeitstempel'], 0, 8));

				for($cnt=$tarray[$dt]; $cnt<$maxtag; $cnt++)
					$darray[1][$cnt]=$t['budget_ges_int_gepl'];
			}
		}
		$darray[1][$maxtag-1]=NoValue;

		// Int. genehm. Gesamtbudget (H)
		$sql="SELECT (budget_pers_int_gen+budget_mat_int_gen) AS budget_ges_int_gen, zeitstempel FROM budget_summe_history WHERE projekt_id=".$vAPID;
		if($vPART < FXP_PRJ_TASK)
			$sql .= " AND aktion=".FXP_CHANGE;
		$sql .= " AND zeitstempel<='".$edate."240000' AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$sql .= " ORDER BY zeitstempel, autoid";
		$tmp=db_values($sql, '*');
//fxDebug($tmp, $sql, 3);
		if(fxIsArray($tmp))
		{
			foreach($tmp as $t)
			{
				$dt=max($sdate, substr((string)$t['zeitstempel'], 0, 8));

				for($cnt=$tarray[$dt]; $cnt<$maxtag; $cnt++)
					$darray[2][$cnt]=$t['budget_ges_int_gen'];
			}
		}
		$darray[2][$maxtag-1]=NoValue;
//fxDebug($darray, '<B>Startdatum:</B> '.$sdate.', <B>Enddatum:</B> '.$edate);

		$darray['col']=array(0=>0x60009f6b, 0xa0006b9f, 0xc0aaaaaa);
//		$darray['col']=array(0=>0x2211aa22, 0x11ee1122, 0x112233ee);
		$darray['leg']=array(0=>get_text(1803, 'text'), get_text(1795, 'text').' (H)', get_text(1797, 'text').' (H)');	// Int. verbr. Gesamtbudget + Int. gepl. Gesamtbudget, Int. genehm. Gesamtbudget

		// Hintergrundfarbe
		$bgcol=0xf0f0f0;

		$bild=bar_chartdir($darray, FXP_CT_LINE, 764, 192, 7, $tt, $GLOBALS['fxpglobals']['settings']['waehrung'], '', $bgcol);
	}
	if(!strlen((string)$bild))
		$bild=bar_horizontal($gpercent, $tt);
	$maskenfeldwerte['verlauf_kosten']='~'.$bild;
}
else
	$Verlauf_Kosten_nv=true;
?>