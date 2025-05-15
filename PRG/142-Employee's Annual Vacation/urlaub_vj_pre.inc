<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : urlaub_vj_pre.inc                                            //
// Version     : 24.1                                                         //
// Begin       : 2020-10-21                                                   //
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
 * PF 142: Employee's Annual Vacation
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

$index=true;

// Get all emplyoees matching the selection criterias
//	$GLOBALS['ignore_personentyp']=true;
$sql  = "SELECT p.personen_id, p.pname, p.vorname, p.geschlecht, p.personentyp, p.inaktiv, m.maeintrittsdatum, m.maaustrittsdatum, m.wochenarbeitstd, m.arbeitstage FROM personen p, madaten m";
$sql .= " WHERE p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.personentyp=".FXP_PT_EMPLOYEE." AND m.personen_id=p.personen_id AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
// ..."Search text"
if(strlen((string)$suchtext))
	$sql .= " AND (".db_like('p.pname','%'.$suchtext.'%')." OR ".db_like('p.vorname','%'.$suchtext.'%').")";
// ..."Department"
if($abt_id > 0)
{
	$acsv=getDepartments($GLOBALS['fxpglobals']['client'], $abt_id);
	$sql .= " AND p.abt_id IN (".$acsv.")";
}
// ..."Time Span from/to"
if(strlen((string)$zeitspanne_von) && strlen((string)$zeitspanne_bis))
	$sql .= " AND m.maeintrittsdatum<='".$zeitspanne_bis."240000' AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$zeitspanne_von."000000')";
else if(strlen((string)$zeitspanne_von))
	$sql .= " AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$zeitspanne_von."000000')";
else if(strlen((string)$zeitspanne_bis))
	$sql .= " AND m.maeintrittsdatum<='".$zeitspanne_bis."240000'";
// ..."Active Employee"
if($akt_ma)
	$sql .= " AND (p.inaktiv IS NULL OR p.inaktiv=0) AND (m.maaustrittsdatum IS NULL OR m.maaustrittsdatum>='".$GLOBALS['date']."000000')";
//$sql .= filter_person();
$sql .= " ORDER BY p.pname, p.vorname";
$personen=db_values($sql, '*');
fxDebug($personen,$sql, 2);

// Persons found?
if(fxIsArray($personen))
{
	$pids=array_to_csv($personen,'personen_id');

	// NoWrap fields
	$Jahr_nw=true;
	$Urlaubsanspruch_nw=true;
	$Urlaub_vj_nw=true;
	$Status_Aktiv_nw=true;

	// Current year
	$aj=(int)substr((string)$GLOBALS['date'],0,4);

	// Text values
	$va=explode('|', meldung(1887,false));	// Kein definierter Startwert vorhanden|Definierter Startwert|Kalkulierter Zwischenwert|Kalkulierter neuer Wert
	$ina='&nbsp;&nbsp;{'.meldung(1594, false).'}';	// inactive

	// Field widths
	$wf=64;
	$ws=$wf+24;
	$wt=$ws*2;

	// Line height
	$Jahr_height=24;

	// Mask values
	$mc=$GLOBALS['_maskcounter'];
	$mn=125;
	$ln=0;

	// Cycle through persons
	foreach($personen as $pers)
	{
		$pid=(int)$pers['personen_id'];

		// Get vacation calculations
		$ru=fxf_getVacCalc($pid);
		// ...Reload adjusted values if in Change or Delete mode
		if(($GLOBALS['fxpglobals']['action'] != FXP_DISPLAY) && fxIsArray($ru))
		{
			$reload=false;
			foreach($ru as $uj => $ud)
			{
				if($ud['dbsn_ko'] || $ud['dbsn_ok'] || $ud['dbvn_ko'] || $ud['dbvn_ok'])
				{
					$reload=true;
					break;
				}
			}
			if($reload)
				$ru=fxf_getVacCalc($pid);
		}
//fxDebug($ru, $pid, 0);

		// Empty line with Top Border?
		if($ln)
		{
			fxf_maskColumnBorder(array('Jahr','Urlaubsanspruch','Urlaub_vj','Urlaub_gn','Status_Aktiv'), 1, -1, $mc, $ln, true);
			$ln++;
		}

		// Name (stretched over whole line)
		$pname=trim((string)$pers['pname']);
		$vname=trim((string)$pers['vorname']);
		if(strlen((string)$vname))
			$name=$pname.', '.$vname;
		else
			$name=$pname;

		$datum=get_entry($pers['maeintrittsdatum'], 'datum');
		$edatum='';
		if($pers['maaustrittsdatum'])
		{
			$edatum=substr((string)$pers['maaustrittsdatum'], 0, 8);
			$datum .= '&nbsp;-&nbsp;'.get_entry($pers['maaustrittsdatum'], 'datum');
		}

		$dclass='black';
		if(strlen((string)$edatum))
		{
			if($edatum < $GLOBALS['date'])
				$dclass='red';
			else
				$dclass='blue';
		}

		$iclass='';
		$itxt='';
		if($pers['inaktiv'])
		{
			$iclass=' class="red"';
			$itxt=$ina;
		}
//$itxt .= ' ($pid='.$pid.')';

		$txt='&nbsp;'.person_pic($pid, 20, $pers['geschlecht'], $GLOBALS['fxpglobals']['client'], false, 'border-top-left-radius:6px;').'&nbsp;&nbsp;<b'.$iclass.'>'.$name.'</b>&nbsp;<i class="'.$dclass.'">['.$datum.']</i>'.$itxt.'&nbsp;';
		fxf_listSectHeader($mc, $ln, $txt, 'jahr', 'status_aktiv', 5, 0, 1);
		$ln++;

		$dlc=0;
		if(fxIsArray($ru))
		{
			foreach($ru as $uj => $ud)
			{
				// If calculations should not be displayed only show defined start values, skip everything else
				if(($ud['type'] != 1) && !$historie)
					continue;

				// 1=Defined start value
				if($ud['type'] == 1)
					$cls='';
				// 3=Calculated new value
				else if($ud['type'] == 3)
					$cls=' class="lightblue"';
				else
					$cls=' class="grey"';

				// Only defined start values can be deleted
				$dcb='';
				if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && ($ud['type'] == 1) && $dlc)
				{
					$chk=false;
					if($err && $_POST['Jahr'][$mc][$ln])
						$chk=true;
					$dcb=fieldCheckbox('Jahr['.$mc.']['.$ln.']', 1, $chk, false, false, false, '', '', '', false, 'position:absolute;left:0;top:-2px;');
				}

				// Year (Jahr)
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']='^class_?" height="24|~<div style="position:relative;">&nbsp;'.$dcb.'<span'.$cls.'>'.$uj.'</span>&nbsp;</div>';

				// Vacation per Year (Urlaubsanspruch)
				$ua=$ud['def_vacationperyear'];
				if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
					$txt='~&nbsp;<span'.$cls.'>'.get_entry($ua, 'dezimal').'&nbsp;d</span>&nbsp;';
				else
				{
					$dua=get_entry($ua, 'dezimal');
					$nua='';
					if($err)
						$nua=$_POST['Urlaubsanspruch'][$mc][$ln];
					else if($ud['type'] == 1)
						$nua=$dua;
					$txt  = '~<table width="'.$wt.'" border="0" cellpadding="0" cellspacing="0"><tr>';
					$txt .= '<td width="'.$ws.'" nowrap align="right">&nbsp;<i'.$cls.'>('.$dua.'&nbsp;d)</i>&nbsp;&nbsp;</td>';
					$txt .= '<td nowrap align="right">'.fieldText('Urlaubsanspruch['.$mc.']['.$ln.']',$nua,$wf,10,'right').'&nbsp;<span'.$cls.'>d</span>&nbsp;</td>';
					$txt .= '</tr></table>';
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaubsanspruch']=$txt;

				// Carry from Prv. Year (Urlaub_vj)
				$uu=$ud['vacationcarry'];
				if(($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY) || ($GLOBALS['fxpglobals']['action'] == FXP_DELETE))
					$txt='~&nbsp;<span'.$cls.'>'.get_entry($uu, 'dezimal').'&nbsp;d</span>&nbsp;';
				else
				{
					$duu=get_entry($uu, 'dezimal');
					$nuu='';
					if($err)
						$nuu=$_POST['Urlaub_vj'][$mc][$ln];
					else if($ud['type'] == 1)
						$nuu=$duu;
					$txt  = '~<table width="'.$wt.'" border="0" cellpadding="0" cellspacing="0"><tr>';
					$txt .= '<td width="'.$ws.'" nowrap align="right">&nbsp;<i'.$cls.'>('.$duu.'&nbsp;d)</i>&nbsp;&nbsp;</td>';
					$txt .= '<td nowrap align="right">'.fieldText('Urlaub_vj['.$mc.']['.$ln.']',$nuu,$wf,10,'right').'&nbsp;<span'.$cls.'>d</span>&nbsp;</td>';
					$txt .= '</tr></table>';
				}
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaub_vj']=$txt;

				// Vav. taken (Urlaub_gn)
				$ug=$ud['vacationtaken'];
				$txt='~&nbsp;<span'.$cls.'>'.get_entry($ug, 'dezimal').'&nbsp;d</span>&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaub_gn']=$txt;

				// Active (Status_Aktiv)
				// ...Sum
				if($ud['dbsn_ko'])				// Neccessary correction: ERROR
					$simg='tl_red';
				else if($ud['dbsn_ok'])			// Neccessary correction: SUCCESS
					$simg='tl_green';
				else if(fxIsArray($ud['dbss']))	// Suggested correction
					$simg='tl_yellow';
				else if($ud['type'] == 1)		// Defined value
					$simg='tl_grey';
				else							// Calculated value
					$simg='tl_neutral';
				// ...Vac
				if($ud['dbvn_ko'])				// Neccessary correction: ERROR
					$vimg='tl_red';
				else if($ud['dbvn_ok'])			// Neccessary correction: SUCCESS
					$vimg='tl_green';
				else							// Calculated value
					$vimg='tl_neutral';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['status_aktiv']='~##'.$ud['tooltip'].'|~<img src="'.$GLOBALS['gfxpath'].'tts.png" align="right">&nbsp;<img src="'.$GLOBALS['gfxpath'].$simg.'.png">&nbsp;<img src="'.$GLOBALS['gfxpath'].$vimg.'.png">&nbsp;&nbsp;<span'.$cls.'>'.$va[$ud['type']].'</span>&nbsp;&nbsp;&nbsp;';

				// Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['personen_id']=$pid;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['pname']=$name;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['artikel_id']=$ud['id'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['archiv']=$ud['dba'];
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_ganzzahl1']=$uj;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_zahl1']=$ua;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_zahl2']=$uu;

				$ln++;

				if($ud['type'] == 1)
					$dlc++;
			}
		}

		if(!$dlc)
		{
			// Create new defined start value
			if($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)
			{
				$cls=' class="red"';
				$uj=(int)substr((string)$pers['maeintrittsdatum'],0,4);

				// Year (Jahr)
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']='^class_?" height="24|~<div style="position:relative;"><span'.$cls.'>'.$uj.'</span>&nbsp;</div>';
				// Vacation per Year (Urlaubsanspruch)
				$ua=0.0;
				$dua=get_entry($ua, 'dezimal');
				$nua=$dua;
				if($err)
					$nua=$_POST['Urlaubsanspruch'][$mc][$ln];
				$txt  = '~<table width="'.$wt.'" border="0" cellpadding="0" cellspacing="0"><tr>';
				$txt .= '<td width="'.$ws.'" nowrap align="right">&nbsp;<i'.$cls.'>('.$dua.'&nbsp;d)</i>&nbsp;&nbsp;</td>';
				$txt .= '<td nowrap align="right">'.fieldText('Urlaubsanspruch['.$mc.']['.$ln.']',$nua,$wf,10,'right').'&nbsp;<span>d</span>&nbsp;</td>';
				$txt .= '</tr></table>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaubsanspruch']=$txt;
				// Carry from Prv. Year (Urlaub_vj)
				$uu=0.0;
				$duu=get_entry(0.0, 'dezimal');
				$nuu=$duu;
				if($err)
					$nuu=$_POST['Urlaub_vj'][$mc][$ln];
				$txt  = '~<table width="'.$wt.'" border="0" cellpadding="0" cellspacing="0"><tr>';
				$txt .= '<td width="'.$ws.'" nowrap align="right">&nbsp;<i'.$cls.'>('.$duu.'&nbsp;d)</i>&nbsp;&nbsp;</td>';
				$txt .= '<td nowrap align="right">'.fieldText('Urlaub_vj['.$mc.']['.$ln.']',$nuu,$wf,10,'right').'&nbsp;<span>d</span>&nbsp;</td>';
				$txt .= '</tr></table>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaub_vj']=$txt;
				// Vav. taken (Urlaub_gn)
				$ug=0.0;
				$txt='~&nbsp;<span'.$cls.'>'.get_entry($ug, 'dezimal').'&nbsp;d</span>&nbsp;';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['urlaub_gn']=$txt;
				// Active (Status_Aktiv)
				// ...Sum
				$simg='tl_red';
				$vimg='tl_red';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['status_aktiv']='~&nbsp;<img src="'.$GLOBALS['gfxpath'].$simg.'.png">&nbsp;<img src="'.$GLOBALS['gfxpath'].$vimg.'.png">&nbsp;&nbsp;<span'.$cls.'>'.$va[0].'!</span>&nbsp;&nbsp;&nbsp;';

				// Hidden
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['personen_id']=$pid;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['pname']=$name;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['artikel_id']=0;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['archiv']=0;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_ganzzahl1']=$uj;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_zahl1']=$ua;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['nv_zahl2']=$uu;

				$ln++;
			}
			// No defined start value exists!
			else
			{
				$txt='<i class=red>'.$va[0].'!</i>';
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['jahr']='^class_?" colspan="5" align="center|~<!--'.$txt;
				$GLOBALS['fxptdata']['lvalues'][$mc][$ln]['status_aktiv']='~-->'.$txt;

				$ln++;
			}
		}
	}

	// Empty line with Top Border?
	if($ln)
	{
		fxf_maskColumnBorder(array('Jahr','Urlaubsanspruch','Urlaub_vj','Urlaub_gn','Status_Aktiv'), 1, -1, $mc, $ln, true);
		$ln++;
	}
}

// Buttons ausblenden?
if($GLOBALS['fxpglobals']['action'] == FXP_DISPLAY)
	$Button_Multi_nv=true;
?>