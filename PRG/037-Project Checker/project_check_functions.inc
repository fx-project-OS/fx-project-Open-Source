<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : project_check_functions.inc                                  //
// Version     : 21.1                                                         //
// Begin       : 2020-12-14                                                   //
// Last Change : 2020-12-14                                                   //
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
 * 37: Project Checker Function Collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $mpid   - Mandatory parameter:                           ???PARAMETER???
 * @param string  $iea    - Optional parameter (default = ''):             ???PARAMETER???
 * @param string  $barr   - Optional parameter (default = ''):             ???PARAMETER???
 * @param string  $rarr   - Optional parameter (default = ''):             ???PARAMETER???
 * @param string  $mode   - Optional parameter (default = 'transaction'):  ???PARAMETER???
 * @param integer $sortid - Optional parameter (default = 0):              ???PARAMETER???
 * @param integer $auto   - Optional parameter (default = 0):              ???PARAMETER???
 *
 * @return ???RETURN???
 */
function projectCheck($mpid, $iea='', $barr='', $rarr='', $mode='transaction', $sortid=0, $auto=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// intern/extern - Array
	if(!fxIsArray($iea))
		$iea=array('int', 'ext');

	// Budgetinfos ermitteln
	if(!fxIsArray($barr))
		$barr=getBudgetFieldArray();

	// Rechnungsinfos ermitteln
	if(!fxIsArray($rarr))
		$rarr=getRechnungFieldArray();

	// Prefix
	getAllPrefixes();
	$prefix='';
	if(isset($GLOBALS['mp_prefixes'][$mpid]))
		$prefix=$GLOBALS['mp_prefixes'][$mpid];
//echo('$mpid='.$mpid.', $prefix='.$prefix.'<hr />');

	$pinf=array();
	if($mpid)
	{
		// Vorgangsnummer des Mutterprojektes ermitteln
		$sql="SELECT vorgangsnummer FROM projekte WHERE projekt_id=".(int)$mpid." AND mutterprojekt_id=".(int)$mpid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
		$mpvn=db_value($sql);
//fxDebug($mpvn, $sql);
		if(strlen($mpvn))
			$mpvn=substr('0000'.substr(trim($mpvn),0,4),-4);
//fxDebug($mpvn, '$mpvn');

		// Projektdaten des Mutterprojektes und aller Nachfolger ermitteln (ohne ToDos)
		if(fxIsArray($tmp=getProjectData($mpid,$mpvn,$sortid)))
		{
			// ...Durchnummerierung festlegen
			$cnt=0;
			foreach($tmp as $tk => $ta)
			{
				$tmp[$tk]['fix']=false;

				// Aktuelle VN ermitteln (DB-Format) 
				$vd=add_null(trim($ta['vorgangsnummer']));

				// VN von Mutterprojekten oder von bereits abgerechneten Projekten bleiben bei Durchnummerierung bestehen ...
				if(($cnt ==  0) || ($ta['budget_pers_ext_abger'] > 0.0) || ($ta['budget_mat_ext_abger'] > 0.0))
				{
					$tmp[$tk]['fix']=true;

					if($cnt > 0)
					{
						$aln=substr($vd,0,-5);
						while(strlen($aln) > 5)
						{
							$ncnt=0;
							foreach($tmp as $ntk => $nta)
							{
								if($ncnt >= $cnt)
									break;

								if($ncnt > 0)
								{
									$nln=add_null(trim($nta['vorgangsnummer']));
									if($aln == $nln)
									{
										$tmp[$ntk]['fix']=true;
										break;
									}
								}
								$ncnt++;
							}
							$aln=substr($aln,0,-5);
						}
					}
				}
				$cnt++;
			}

			// ...Daten ermitteln
			$cnt=0;
			$ea=array();
			$la=array();
			$le=array();
			$va=array();
			$sa=array();
			$lp=0;
			$lt=0;
			$ll=0;
			foreach($tmp as $tk => $ta)
			{
				$pid=(int)$ta['projekt_id'];

				// Aktuelle Nummer
				$vd=add_null(trim($ta['vorgangsnummer']));

				// Elter + Level
				if(!$cnt)
				{
					$ea[$pid]=0;
					$la[$pid]=0;
					$le[$la[$pid]]=$pid;

					$vk=substr('0000'.$vd,-4);
					$vf=$vk;
				}
				else
				{
					$pl=getLevel($vd);
					if($sortid)
					{
						$ea[$pid]=$ta['elter'];
						$la[$pid]=$la[$ea[$pid]]+1;
						$le[$la[$pid]]=$pid;
					}
					else
					{
						$la[$pid]=min($ll+1,$pl);
						$le[$la[$pid]]=$pid;
						$ea[$pid]=$le[$la[$pid]-1];
					}

					// Aufgabe folgt Aufgabe?
					if(($ta['projektart'] == FXP_PRJ_TASK) && ($la[$pid] > $ll) && ($lt == FXP_PRJ_TASK))
					{
						$ea[$pid]=$ea[$ea[$pid]];
						$la[$pid]=$la[$ea[$pid]]+1;
						$le[$la[$pid]]=$pid;
					}

					// Korrekte Nummer
					$ve=$va[$ea[$pid]].'.';
					$vl=strlen($ve);
					$v4=$vl+4;
					$mk=1;
					foreach($va as $k)
					{
						$ks=substr($k,0,$vl);
						if(($ks == $ve) && (strlen($k) == $v4))
							$mk=(int)substr($k,-4);
						else if($ks > $ve)
							break;
					}
					$ak=max($mk,(int)substr($vd,-4));
					while(true)
					{
						$vk=$ve.substr('000'.$ak,-4);
						if(!isset($sa[$vk]))
							break;
						$ak++;
					}

					// Fortlaufende Nummer
					if($vk != $vd)
						$vf=$vk;
					else if($ta['fix'])
						$vf=$vd;
					else
						$vf=getValidVorgangsnummer($vf,$la[$pid],true);
				}
				$lp=$pid;
				$lt=$ta['projektart'];
				$ll=$la[$pid];
				$va[$pid]=$vk;

				if($auto)
					$sa[$vf]=array($pid, $vd, $vk, $vf, $tk);
				else
					$sa[$vk]=array($pid, $vd, $vk, $vf, $tk);

/*
{
if($ta['projektart'] <= FXP_PRJ_SUB)
	echo('<div style="background:#dddddd;margin-top:1px;">');
echo($pid.': $vd='.$vd.', $ea['.$pid.']='.$ea[$pid].', $la['.$pid.']='.$la[$pid].' -- $vk=');
if($vk != $vd)
	echo('<b class="red">'.$vk.'</b>');
else
	echo('<font class="green">'.$vk.'</font>');
echo(' -- $vf=');
if($vf != $vd)
	echo('<b class="red">'.$vf.'</b>');
else if($ta['fix'])
	echo('<font class="blue">'.$vf.' (fix)</font>');
else
	echo('<font class="green">'.$vf.'</font>');
if($ta['projektart'] <= FXP_PRJ_SUB)
	echo('</div>'.$nl);
else
	echo('<br />'.$GLOBALS['nl']);
}
*/
				$cnt++;
			}

			ksort($sa,SORT_STRING);
//fxDebug($sa,'$sa');

			foreach($sa as $v => $s)
			{
				$pid=$s[0];

				// Projekt-Array für diesen Vorgang erstellen [function getProjektArray($lvl, $elter, $vd, $vk, $vf, $t)]
				$pinf[$pid]=getProjektArray($mpid, $ea[$pid], $la[$pid], $s[1], $s[2], $s[3], $tmp[$s[4]]);	// Mutterprojekt, Elter, Level, DB-Nummer, K-Nummer, F-Nummer, Data-Array

				// Sollaufwände hochaddieren
				$aufw=(double)$pinf[$pid]['p']['aufwand_soll'];
				$part=(int)$pinf[$pid]['p']['projektart'];
				if(($part == FXP_PRJ_TASK) && ($aufw > 0.0))
				{
					do
					{
						$pinf[$pid]['as'] += $aufw;
						$pid=$pinf[$pid]['e'];
					} while($pid && fxIsArray($pinf[$pid]));
				}
			}

			unset($tmp);
		}
	}

	if(sizeof($pinf))
	{
		// Budgetdaten ermitteln
		if(fxIsArray($tmp=getBudgetDaten($mpid,$mpvn,$sortid)))
		{
			foreach($tmp as $t)
			{
				$pid=(int)$t['projekt_id'];
				if(!fxIsArray($pinf[$pid]))
					continue;

				// Gepl. Budgets von Aufgaben zu allen übergeordneten Vorgängen hochaddieren
				addBudgetPlan($t, $pinf, $barr);
			}

			foreach($tmp as $t)
			{
				$pid=(int)$t['projekt_id'];
				if(!fxIsArray($pinf[$pid]))
					continue;

				addBudgetRahmen($t, $pinf);
			}

			// Budgetrahmen anpassen
			adjustBudgetRahmen($pinf);

			unset($tmp);
		}

		// Zeitinfos ermitteln
		if(fxIsArray($tmp=getZeitDaten($mpid,$mpvn)))
		{
			foreach($tmp as $t)
				setZeitDaten($t, $pinf);

			unset($tmp);
		}

		// genehmigt + aktiviert (nur auf Aufgaben)
		if(fxIsArray($tmp=getGenAktBudget($mpid,$mpvn)))
		{
			foreach($tmp as $t)
				setGenAktBudget($t, $pinf);

			unset($tmp);
		}

		// Ist-Daten ermitteln für Personalkosten (Zeitraum, Zeitaufwand)
		if(fxIsArray($tmp=getIstDatenPers($mpid,$mpvn)))
		{
			foreach($tmp as $t)
				addIstDatenPers($t, $pinf);

			unset($tmp);
		}

		// Ist-Daten ermitteln für Sofortmaterial
		if(fxIsArray($tmp=getIstDatenSofortmaterial($mpid,$mpvn)))
		{
			foreach($tmp as $t)
				addIstDatenSofortmaterial($t, $pinf);

			unset($tmp);
		}

		// Ist-Daten ermitteln für Zeit- und Verbrauchsmaterial
		if(fxIsArray($tmp=getIstDatenVerbrauchsmaterial($mpid,$mpvn)))
		{
			foreach($tmp as $t)
				addIstDatenVerbrauchsmaterial($t, $pinf);

			unset($tmp);
		}

		// bereits abgerechnete Werte ermitteln
		if(fxIsArray($tmp=getRechnungsDaten($mpid,$mpvn)))
		{
			foreach($tmp as $t)
				addRechnungsDaten($t, $pinf);

			unset($tmp);
		}
	}
	if($mode == 'transaction')
		return $pinf;

	$diffs=getProjectDifferences($pinf, $iea, $barr, $rarr);
//fxDebug($diffs, '$diffs in '.$mpid);
	return correctValues($diffs);
}

/**
 * ???FUNCTION???
 *
 * @param various $toDo - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function correctValues($toDo)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$completedWithErrors=false;
	if(fxIsArray($toDo['pc']) || fxIsArray($toDo['bc']) || fxIsArray($toDo['zc']))
	{
		$err=false;
		foreach($toDo as $typ => $arr)
		{
			// Keine Änderungen notwendig, dann NEXT
			if(!fxIsArray($arr))
				continue;

			foreach($arr as $pid => $fa)
			{
				$chk="";
				$where=" WHERE projekt_id=".$pid." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				switch($typ)
				{
					case 'pc';	// Änderungen an Projekten durchführen
						$chk="SELECT projekt_id FROM projekte".$where;
						$sql="UPDATE projekte SET ";
					break;

					case 'bc';	// Änderungen an Budgets durchführen
						$chk="SELECT projekt_id FROM budget_summe".$where;
						$sql="UPDATE budget_summe SET ";
					break;

					case 'zc';	// Änderungen an Zeiten durchführen
						$where .= " AND zeitart=102";
						$chk="SELECT zeit_id FROM zeitdaten".$where;
						$sql="UPDATE zeitdaten SET ";
					break;
				}

				// Nicht unterstützter Typ, dann NEXT
				if(!strlen($chk))
					continue;

				// Datensatz nicht mehr vorhanden, dann NEXT
				$exists=(int)db_value($chk);
				if(!$exists)
				{
					// besser evtl. deleteProcesses() am Ende ...
					continue;
				}

				// Update-SQL zusammenbauen
				$fc=0;
				foreach($fa as $field => $value)
				{
					if(strlen($value))
					{
						if($fc)
							$sql .= ", ";
						$sql .= $field."=".$value;

						$fc++;
					}
				}
				$sql .= $where;
//fxDebug($sql, $typ.': '.$pid);

				if($fc)
				{
					$err=db_x($sql);
					if($err)
					{
						$completedWithErrors=true;
						break 2;
					}
				}
			}
		}
	}
	return $completedWithErrors;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function getBudgetFieldArray()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return array
	(
		"ibpp"=>'budget_pers_int_gepl',		"ibpg"=>'budget_pers_int_gen',		"ibpa"=>'budget_pers_int_akt',
		"ibpv"=>'budget_pers_int_verpl',	"ibpb"=>'budget_pers_int_verbr',

		"ebpp"=>'budget_pers_ext_gepl',		"ebpg"=>'budget_pers_ext_gen',		"ebpa"=>'budget_pers_ext_akt',
		"ebpv"=>'budget_pers_ext_verpl',	"ebpb"=>'budget_pers_ext_verbr',	"ebpc"=>'budget_pers_ext_abger',

		"ibmp"=>'budget_mat_int_gepl',		"ibmg"=>'budget_mat_int_gen',		"ibma"=>'budget_mat_int_akt',
		"ibmv"=>'budget_mat_int_verpl',		"ibmb"=>'budget_mat_int_verbr',

		"ebmp"=>'budget_mat_ext_gepl',		"ebmg"=>'budget_mat_ext_gen',		"ebma"=>'budget_mat_ext_akt',
		"ebmv"=>'budget_mat_ext_verpl',		"ebmb"=>'budget_mat_ext_verbr',		"ebmc"=>'budget_mat_ext_abger'
	);
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function getRechnungFieldArray()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return array
	(
		"rfe"=>'rechnung_fest_ext',			"rse"=>'rechnung_std_ext',			"rre"=>'rechnung_reise_ext',		"rme"=>'rechnung_mat_ext',
		"rfeo"=>'rechnung_fest_ext_off',	"rseo"=>'rechnung_std_ext_off',		"rreo"=>'rechnung_reise_ext_off',	"rmeo"=>'rechnung_mat_ext_off'
	);
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid  - Mandatory parameter: ???PARAMETER???
 * @param various $elter - Mandatory parameter: ???PARAMETER???
 * @param various $lvl   - Mandatory parameter: ???PARAMETER???
 * @param various $vd    - Mandatory parameter: ???PARAMETER???
 * @param various $vk    - Mandatory parameter: ???PARAMETER???
 * @param various $vf    - Mandatory parameter: ???PARAMETER???
 * @param various $t     - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getProjektArray($mpid, $elter, $lvl, $vd, $vk, $vf, $t)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pf='';
	$pl=0;
	$nm=trim($t['name_projekt']);
	if(fxIsArray($GLOBALS['mp_prefixes']) && isset($GLOBALS['mp_prefixes'][$mpid]))
	{
		$pf=trim($GLOBALS['mp_prefixes'][$mpid]);
		$pl=strlen($pf);
	}
	if($pl && (strtolower(substr($nm,0,$pl)) == strtolower($pf)))
		$nm=trim(substr($nm,$pl));
	if(!strlen($nm))
	{
		$sql="SELECT autoid, name_projekt FROM projekte_history WHERE projekt_id=".$t['projekt_id']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']." ORDER BY autoid DESC";
		$hta=db_values($sql,'*');
		if(fxIsArray($hta))
		{
			foreach($hta as $ht)
			{
				$nm=trim($ht['name_projekt']);
				if($pl && (strtolower(substr($nm,0,$pl)) == strtolower($pf)))
					$nm=trim(substr($nm,$pl));
				if(strlen($nm))
					break;
			}
		}
	}
	if(!strlen($nm))
		$nm='Dummy '.$GLOBALS['fxpglobals']['client'].'-'.$t['projekt_id'];
	if(strlen($pf))
		$pf=trim($pf).' ';

	return array
	(
		'm'		=> $mpid,
		'e'		=> $elter,
		'l'		=> $lvl,
		'vd'	=> $vd,		'vk'	=> $vk,		'vf'	=> $vf,

		'pf'	=> $pf,		'nm'	=> $nm,		'pn'	=> $pf.$nm,

		'abr'	=> 639,

		'sb'	=> '',		'se'	=> '',		'ib'	=> '',		'ie'	=> '',
		'iv'	=> 0.0,

		'as'	=> 0.0,	'ai'	=> 0.0,

		'ibpr'	=> 0.0,	'ibmr'	=> 0.0,
		'ebpr'	=> 0.0,	'ebmr'	=> 0.0,

		'ibpp'	=> 0.0,	'ibpg'	=> 0.0,	'ibpa'	=> 0.0,	'ibpv'	=> 0.0,	'ibpb'	=> 0.0,
		'ebpp'	=> 0.0,	'ebpg'	=> 0.0,	'ebpa'	=> 0.0,	'ebpv'	=> 0.0,	'ebpb'	=> 0.0,	'ebpc'	=> 0.0,

		'ibmp'	=> 0.0,	'ibmg'	=> 0.0,	'ibma'	=> 0.0,	'ibmv'	=> 0.0,	'ibmb'	=> 0.0,
		'ebmp'	=> 0.0,	'ebmg'	=> 0.0,	'ebma'	=> 0.0,	'ebmv'	=> 0.0,	'ebmb'	=> 0.0,	'ebmc'	=> 0.0,

		'rfe'	=> 0.0,	'rse'	=> 0.0,	'rre'	=> 0.0,	'rme'	=> 0.0,
		'rfeo'	=> 0.0,	'rseo'	=> 0.0,	'rreo'	=> 0.0,	'rmeo'	=> 0.0,

		'tp'	=> array(),

		'tai'	=> 0.0,
		'tibpp'	=> 0.0,	'tebpb'	=> 0.0,
		'trseo'	=> 0.0,	'trreo'	=> 0.0,

		'p'		=> $t,
		'b'		=> false,
		'z'		=> false
	);
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid   - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn   - Mandatory parameter: ???PARAMETER???
 * @param various $sortid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getProjectData($mpid, $mpvn, $sortid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT";
	$sql .= " p.mutterprojekt_id, p.projekt_id, p.elter, p.vorgangsnummer, p.name_projekt, p.projektart, p.projekt_status, p.aufwand_soll, p.aufwand_ist,";
	$sql .= " b.budget_pers_ext_abger, b.budget_mat_ext_abger ";
	$sql .= "FROM";
	$sql .= " projekte p, budget_summe b ";
	$sql .= "WHERE";
	$sql .= " (p.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR p.vorgangsnummer LIKE '".$mpvn."%'";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projektart<=".FXP_PRJ_TASK;
	$sql .= " AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client']." ";
	$sql .= "ORDER BY";
	$sql .= " p.vorgangsnummer";
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getProjectData()<br />'.$sql);

	if(fxIsArray($tmp) && $sortid)
	{
		$stmp=array();
		$ca=array();
		$pa=array(0=>0);

		while(sizeof($tmp) && sizeof($pa))
		{
			$c=1;
			$na=array();
//fxDebug($pa,'START $pa');
//fxDebug($ca,'START $ca');
			foreach($tmp as $tc => $ta)
			{
				$pid=(int)$ta['projekt_id'];
				$eid=(int)$ta['elter'];
				foreach($pa as $p => $d)
				{
					if((!$p && ($t['projektart'] == FXP_PRJ_MAIN)) || ($eid == $p))
					{
						if(isset($ca[$eid]))
							$ca[$pid]=$ca[$eid].substr('0000'.$c,-5);
						else
							$ca[$pid]=substr('0000'.$c,-5);

						$stmp[$ca[$pid]]=$ta;
						unset($tmp[$tc]);

						if(!isset($na[$pid]))
							$na[$pid]=1;
						else
							$na[$pid]++;

						$c++;
					}
				}
			}

			$pa=$na;
		}

		// Falls es noch nicht zugeordnete Vorgänge gibt, d.h. Vorgänge deren Elter nicht zum Mutterprojekt gehören dann anhand der Vorgangsnummer zuordnen?
//fxDebug($tmp,'Falls es noch nicht zugeordnete Vorgänge gibt, d.h. Vorgänge deren Elter nicht zum Mutterprojekt gehören dann anhand der Vorgangsnummer zuordnen?');
		if(sizeof($tmp))
		{
			foreach($tmp as $tc => $ta)
			{
				$pid=(int)$ta['projekt_id'];
				$vn=$ta['vorgangsnummer'];
				foreach($stmp as $sc => $sa)
				{
					if($sa['vorgangsnummer'] == $vn)
					{
						$c=(int)substr($sc,-5);
						while(true)
						{
							$nc=substr($sc,0,-5).substr('0000'.$c,-5);
							if(!isset($stmp[$nc]))
								break;
							$c++;
						}
						$stmp[$nc]=$ta;
						unset($tmp[$tc]);
					}
				}
			}

			// Falls es immer noch nicht zugeordnete Vorgänge gibt, d.h. Vorgänge deren Elter oder Vorgangsnummer nicht zum Mutterprojekt gehören dann diese ans Mutterprojektende zuordnen?
//fxDebug($tmp,'Falls es immer noch nicht zugeordnete Vorgänge gibt, d.h. Vorgänge deren Elter oder Vorgangsnummer nicht zum Mutterprojekt gehören dann diese ans Mutterprojektende zuordnen?');
			if(sizeof($tmp))
			{
				$c=1;
				foreach($tmp as $tc => $ta)
				{
					while(true)
					{
						$nc='00001'.substr('0000'.$c,-5);
						if(!isset($stmp[$nc]))
							break;
						$c++;
					}
					$stmp[$nc]=$ta;
					unset($tmp[$tc]);
				}
			}
		}

		if(sizeof($stmp))
		{
			ksort($stmp,SORT_STRING);
//fxDebug($stmp,'$stmp');
		}

		return $stmp;
	}

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid   - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn   - Mandatory parameter: ???PARAMETER???
 * @param various $sortid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getBudgetDaten($mpid, $mpvn, $sortid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT";
	$sql .= " b.projekt_id, b.abrechnungsart, b.abrechnungsart_mat,";

	$sql .= " b.budgetrahmen_pers_int, b.budgetrahmen_pers_ext,";
	$sql .= " b.budget_pers_int_gepl, b.budget_pers_int_gen, b.budget_pers_int_akt, b.budget_pers_int_verpl, b.budget_pers_int_verbr,";
	$sql .= " b.budget_pers_ext_gepl, b.budget_pers_ext_gen, b.budget_pers_ext_akt, b.budget_pers_ext_verpl, b.budget_pers_ext_verbr, b.budget_pers_ext_abger,";

	$sql .= " b.budgetrahmen_mat_int, b.budgetrahmen_mat_ext,";
	$sql .= " b.budget_mat_int_gepl, b.budget_mat_int_gen, b.budget_mat_int_akt, b.budget_mat_int_verpl, b.budget_mat_int_verbr,";
	$sql .= " b.budget_mat_ext_gepl, b.budget_mat_ext_gen, b.budget_mat_ext_akt, b.budget_mat_ext_verpl, b.budget_mat_ext_verbr, b.budget_mat_ext_abger,";

	$sql .= " b.rechnung_fest_ext, b.rechnung_std_ext, b.rechnung_reise_ext, b.rechnung_mat_ext,";
	$sql .= " b.rechnung_fest_ext_off, b.rechnung_std_ext_off, b.rechnung_reise_ext_off, b.rechnung_mat_ext_off ";

	$sql .= "FROM";
	$sql .= " budget_summe b, projekte p ";
	$sql .= "WHERE";
	$sql .= " (p.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR p.vorgangsnummer LIKE '".$mpvn."%'";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND p.projektart<=".FXP_PRJ_TASK;
	$sql .= " AND b.projekt_id=p.projekt_id AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getBudgetData()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 * @param various &$budgetFelder - Mandatory parameter: ???PARAMETER???
 */
function addBudgetPlan(&$projekt, &$projektArray, &$budgetFelder)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];
	if($projektArray[$pid]['p']['projektart'] == FXP_PRJ_TASK)
	{
		// gepl. Budgets von Aufgaben zu allen übergeordneten Vorgängen hochaddieren
		foreach($budgetFelder as $bk => $bt)
		{
			if(substr($bk, -1) != 'p')
				continue;
			$bv=(double)$projekt[$bt];
			if($bv > 0.0)
			{
				do
				{
					$projektArray[$pid][$bk] += $bv;
					$pid=$projektArray[$pid]['e'];
				} while($pid && fxIsArray($projektArray[$pid]));
			}
			$pid=(int)$projekt['projekt_id'];
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function addBudgetRahmen(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];

	$projektArray[$pid]['pabr']	= (int)$projekt['abrechnungsart'];
	$projektArray[$pid]['mabr']	= (int)$projekt['abrechnungsart_mat'];
	$projektArray[$pid]['b']	= $projekt;

	// Nur relevant für Mutter- oder Teilprojekte
	if(($projektArray[$pid]['p']['projektart'] == FXP_PRJ_MAIN) || ($projektArray[$pid]['p']['projektart'] == FXP_PRJ_SUB))
	{
		$iea=array('int', 'ext');
		$pma=array('pers', 'mat');
		foreach($iea as $ie)
		{
			foreach($pma as $pm)
			{
				// Budgetrahmen
				$kr=substr($ie,0,1).'b'.substr($pm,0,1).'r';
				$kp=substr($kr,0,3).'p';
				$vr=(double)$projekt['budgetrahmen_'.$pm.'_'.$ie];
				if($vr > 0.0)
				{
					$vp=$projektArray[$pid][$kp];
					$vm=max($vr,$vp);
					$projektArray[$pid][$kr]=$vm;
				}
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function adjustBudgetRahmen(&$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$iea=array('int', 'ext');
	$pma=array('pers', 'mat');
	// Projekte in umgekehrter Reihenfolge ermitteln
	$a=array();
	$c=sizeof($projektArray);
	foreach($projektArray as $pid => $pa)
	{
		if(($pa['p']['projektart'] == FXP_PRJ_MAIN) || ($pa['p']['projektart'] == FXP_PRJ_SUB))
		{
			$a[$c]=$pid;
			$c--;
		}
	}
	rsort($a, SORT_NUMERIC);
	foreach($a as $pid)
	{
		$pa=$projektArray[$pid];
		if(($pa['p']['projektart'] == FXP_PRJ_MAIN) || ($pa['p']['projektart'] == FXP_PRJ_SUB))
		{
			foreach($iea as $ie)
			{
				foreach($pma as $pm)
				{
					$kr=substr($ie,0,1).'b'.substr($pm,0,1).'r';
					$vr=$pa[$kr];
					if($vr > 0.0)
					{
						$kp=substr($kr,0,3).'p';
						// Unterprojekte auf nächstem Level suchen
						$bmin=$pa[$kp];
						$bmax=0.0;
						$sl=$pa['l']+1;
						$sv=$pa['vf'].'.';
						$vl=strlen($sv);
						foreach($projektArray as $spid => $spa)
						{
							if(($spa['l'] == $sl) && (substr($spa['vf'],0,$vl) == $sv))
							{
								if($spa['p']['projektart'] == FXP_PRJ_SUB)
									$badd=max($spa[$kp], $spa[$kr]);
								else
									$badd=$spa[$kp];
								$bmax += $badd;
							}
						}
						$projektArray[$pid][$kr]=max($bmin, max($projektArray[$pid][$kr],$bmax));
					}
				}
			}
		}
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getZeitDaten($mpid, $mpvn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql ="SELECT projekt_id, soll_beginn, soll_ende, ist_beginn, ist_ende, ist_verzoeg";
	$sql .= " FROM zeitdaten";
	$sql .= " WHERE zeitart=102 AND (mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$mpvn."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	$sql .= ") AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getZeitDaten()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function setZeitDaten(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];
	if(fxIsArray($projektArray[$pid]))
	{
		if(strlen($projekt['soll_beginn']))
			$projektArray[$pid]['sb']=substr($projekt['soll_beginn'], 0, 8).'000000';
		if(strlen($projekt['soll_ende']))
			$projektArray[$pid]['se']=substr($projekt['soll_ende'], 0, 8).'240000';
		if(strlen($projekt['ist_beginn']))
			$projektArray[$pid]['ib']=$projekt['ist_beginn'];
		if(strlen($projekt['ist_ende']))
			$projektArray[$pid]['ie']=$projekt['ist_ende'];
		$projekt['ist_verzoeg']=(double)$projekt['ist_verzoeg'];
		$projektArray[$pid]['z']=$projekt;		
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getGenAktBudget($mpid, $mpvn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql ="SELECT b.projekt_id, b.budget_pers_int_gen, b.budget_pers_ext_gen, b.budget_mat_int_gen, b.budget_mat_ext_gen";
	$sql .= " FROM budget_detail b, projekte p";
	$sql .= " WHERE b.projekt_id=p.projekt_id AND (p.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR p.vorgangsnummer LIKE '".$mpvn."%'";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND b.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getGenAktBudget()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function setGenAktBudget(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];
	if(!fxIsArray($projektArray[$pid]) || ($projektArray[$pid]['p']['projektart'] < FXP_PRJ_TASK))
		return;

	$ibpg		= (double)$projekt['budget_pers_int_gen'];
	$ebpg		= (double)$projekt['budget_pers_ext_gen'];
	$ibmg		= (double)$projekt['budget_mat_int_gen'];
	$ebmg		= (double)$projekt['budget_mat_ext_gen'];

	$ibpa		= 0.0;
	$ebpa		= 0.0;
	$ibma		= 0.0;
	$ebma		= 0.0;

	$rfeo		= 0.0;

	if(($projektArray[$pid]['p']['projekt_status'] == FXP_PS_ACTIVE) || ($projektArray[$pid]['p']['projekt_status'] == FXP_PS_COMPLETED))
	{
		$ibpa=$ibpg;
		$ebpa=$ebpg;
		$ibma=$ibmg;
		$ebma=$ebmg;
		if($projektArray[$pid]['b']['abrechnungsart'] == FXP_INV_FIX)		// Festpreis (Personen)
			$rfeo += $ebpa;
		if($projektArray[$pid]['b']['abrechnungsart_mat'] == FXP_INV_FIX)	// Festpreis (Material)
			$rfeo += $ebma;
	}

	do
	{
		$projektArray[$pid]['ibpg'] += $ibpg;
		$projektArray[$pid]['ebpg'] += $ebpg;
		$projektArray[$pid]['ibmg'] += $ibmg;
		$projektArray[$pid]['ebmg'] += $ebmg;

		$projektArray[$pid]['ibpa'] += $ibpa;
		$projektArray[$pid]['ebpa'] += $ebpa;
		$projektArray[$pid]['ibma'] += $ibma;
		$projektArray[$pid]['ebma'] += $ebma;

		$projektArray[$pid]['rfeo']=get_double($projektArray[$pid]['rfeo'])+$rfeo;
		// weiter mit Elter-Projekt-ID
		$pid=$projektArray[$pid]['e'];
	} while($pid && fxIsArray($projektArray[$pid]));
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getIstDatenPers($mpid, $mpvn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql ="SELECT p.projektart, z.projekt_id, z.personen_id, z.zeitart, z.ist_beginn, z.ist_ende, z.zeitaufwand, z.nicht_abrechenbar, z.rk_nicht_abrechenbar, z.kosten_ek, z.kosten_vk, z.rk_ek, z.rk_vk";
	$sql .= " FROM zeitdaten z, projekte p";
	$sql .= " WHERE z.zeitart IN (".FXP_TE_TIMEREC.",".FXP_TE_TIMEREC_SUM.",".FXP_TE_TRAVELEXP_TIMEREC.",".FXP_TE_TRAVELEXP.") AND (z.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR z.projekt_id IN (SELECT projekt_id FROM projekte WHERE vorgangsnummer LIKE '".$mpvn."%' AND mandanten_id=".$GLOBALS['fxpglobals']['client'].")";
	$sql .= ") AND z.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$sql .= " AND p.projekt_id=z.projekt_id AND p.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getIstDatenPers()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function addIstDatenPers(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];
	if(!fxIsArray($projektArray[$pid]))
		return;

	// Personal-SB-kosten + Reisekosten
	$int_pers	= (double)$projekt['kosten_ek'];
	$int_reise	= (double)$projekt['rk_ek'];
	$ext_pers	= 0.0;
	$ext_reise	= 0.0;
	// Rechnung
	$rseo		= 0.0;
	$rreo		= 0.0;

	if(!$projekt['nicht_abrechenbar'])	// Zeitdaten abrechenbar
	{
		if(($projektArray[$pid]['b']['abrechnungsart'] == FXP_INV_FIX) || ($projektArray[$pid]['b']['abrechnungsart'] == FXP_INV_HOUR))	// Festpreis- bzw. Stundenbasis
			$ext_pers=(double)$projekt['kosten_vk'];

		if($projektArray[$pid]['b']['abrechnungsart'] == FXP_INV_HOUR)	// Stundenbasis
			$rseo=$ext_pers;
	}
	if(!$projekt['rk_nicht_abrechenbar'])	// Reisekosten abrechenbar
	{
		$ext_reise=(double)$projekt['rk_vk'];
		$rreo=$ext_reise;
	}

	// Buchung auf Projekt?
	if(($projekt['projektart'] != FXP_PRJ_TASK) && ($projekt['zeitart'] != FXP_TE_TRAVELEXP))
	{
		$tpid=$pid;
		$prs=(int)$projekt['personen_id'];

		$projektArray[$tpid]['tai'] += (double)$projekt['zeitaufwand'];
		$projektArray[$tpid]['tibpb'] += $int_pers + $int_reise;
		$projektArray[$tpid]['tebpb'] += $ext_pers + $ext_reise;
		$projektArray[$tpid]['trseo'] += $rseo;
		$projektArray[$tpid]['trreo'] += $rreo;

		do
		{
			if(!isset($projektArray[$tpid]['tp'][$pid]))
				$projektArray[$tpid]['tp'][$pid]=array();
			if(!isset($projektArray[$tpid]['tp'][$pid][$prs]))
				$projektArray[$tpid]['tp'][$pid][$prs]=array('ct'=>1, 'st'=>$projekt['ist_beginn'], 'en'=>$projekt['ist_ende'], 'ef'=>$projekt['zeitaufwand']);
			else
			{
				$projektArray[$tpid]['tp'][$pid][$prs]['ct']++;
				$projektArray[$tpid]['tp'][$pid][$prs]['st']=min($projektArray[$tpid]['tp'][$pid][$prs]['st'],$projekt['ist_beginn']);
				$projektArray[$tpid]['tp'][$pid][$prs]['en']=max($projektArray[$tpid]['tp'][$pid][$prs]['en'],$projekt['ist_ende']);
				$projektArray[$tpid]['tp'][$pid][$prs]['ef'] +=$projekt['zeitaufwand'];
			}

			$tpid=$projektArray[$tpid]['e'];
		} while($tpid && fxIsArray($projektArray[$tpid]));
	}

	do
	{
		if(strlen($projekt['ist_beginn']) && (!strlen($projektArray[$pid]['ib']) || ($projektArray[$pid]['ib'] > $projekt['ist_beginn'])))
			$projektArray[$pid]['ib']=$projekt['ist_beginn'];
		if(strlen($projekt['ist_ende']) && (($projektArray[$pid]['p']['projekt_status'] == FXP_PS_COMPLETED) && (!strlen($projektArray[$pid]['ie']) || ($projektArray[$pid]['ie'] < $projekt['ist_ende']))))
			$projektArray[$pid]['ie']=$projekt['ist_ende'];

		if($projekt['zeitart'] != FXP_TE_TRAVELEXP)	// Zeiterfassungsdaten
		{
			$projektArray[$pid]['ai'] += (double)$projekt['zeitaufwand'];
			$projektArray[$pid]['iv'] += (double)$projekt['ist_verzoeg'];
		}

		$projektArray[$pid]['ibpb'] += $int_pers + $int_reise;
		$projektArray[$pid]['ebpb'] += $ext_pers + $ext_reise;

		$projektArray[$pid]['rseo'] += $rseo;
		$projektArray[$pid]['rreo'] += $rreo;

		$pid=$projektArray[$pid]['e'];
	} while($pid && fxIsArray($projektArray[$pid]));
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getIstDatenSofortmaterial($mpid, $mpvn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT s.fuervorgang, s.sachmittelstatus, s.name_sachmittel, s.sachmenge, s.soll_kosten, s.anschaffungspreis, s.verkaufspreis, s.soll_ende";
	$sql .= " FROM sachmittel s, projekte p";
	$sql .= " WHERE s.verwendungsart=3149 AND s.fuervorgang=p.projekt_id AND (p.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR p.vorgangsnummer LIKE '".$mpvn."%'";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND s.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getIstDatenSofortmaterial()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function addIstDatenSofortmaterial(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['fuervorgang'];
	if(!fxIsArray($projektArray[$pid]))
		return;

	$st=(int)$projekt['sachmittelstatus'];
	if(($st != 1237) && ($st != 1238))
		return;

	$int_mat=0.0;
	$ext_mat=(double)$projekt['sachmenge'] * (double)$projekt['verkaufspreis'];
	$rmeo=0.0;

	if($st == 1238)	// bestellt
	{
		$vb			= 'v';
		$int_mat	= (double)$projekt['sachmenge'] * (double)$projekt['soll_kosten'];
	}
	else			// bezahlt
	{
		$vb			= 'b';
		$int_mat	= (double)$projekt['sachmenge'] * (double)$projekt['anschaffungspreis'];

		if(strlen($projekt['soll_ende']) > 7)
			$rmeo=(double)$projekt['sachmenge'] * (double)$projekt['verkaufspreis'];
	}

	do
	{
		$projektArray[$pid]['ibm'.$vb]	+= $int_mat;
		$projektArray[$pid]['ebm'.$vb]	+= $ext_mat;

		$projektArray[$pid]['rmeo']		+= $rmeo;

		$pid=$projektArray[$pid]['e'];
	} while($pid && fxIsArray($projektArray[$pid]));
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getIstDatenVerbrauchsmaterial($mpid, $mpvn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql  = "SELECT m.projekt_id, m.sachmenge, m.kosten_ek, m.kosten_vk";
	$sql .= " FROM materialverbrauch m, projekte p";
	$sql .= " WHERE m.projekt_id=p.projekt_id AND (p.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR p.vorgangsnummer LIKE '".$mpvn."%'";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND m.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getIstDatenVerbrauchsmaterial()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function addIstDatenVerbrauchsmaterial(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];
	if(!fxIsArray($projektArray[$pid]))
		return;

	$int_mat=(double)$projekt['sachmenge'] * (double)$projekt['kosten_ek'];
	$ext_mat=(double)$projekt['sachmenge'] * (double)$projekt['kosten_vk'];

	$rseo=0.0;
	if($projektArray[$pid]['b']['abrechnungsart_mat'] == FXP_INV_MAT_HU)
		$rseo=$ext_mat;

	do
	{
		$projektArray[$pid]['ibmb']	+= $int_mat;
		$projektArray[$pid]['ebmb']	+= $ext_mat;

		$projektArray[$pid]['rseo']	+= $rseo;

		$pid=$projektArray[$pid]['e'];
	} while($pid && fxIsArray($projektArray[$pid]));
}

/**
 * ???FUNCTION???
 *
 * @param various $mpid - Mandatory parameter: ???PARAMETER???
 * @param various $mpvn - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getRechnungsDaten($mpid, $mpvn)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$sql ="SELECT rp.rechnungs_id, rp.positions_id, rp.projekt_id, rp.gesamtpreis, rp.ist_sachmittel";
	$sql .= " FROM rechnung r, rechnungsposition rp, projekte p";
	$sql .= " WHERE r.rechnungs_id=rp.rechnungs_id AND (r.archiv IS NULL OR r.archiv=0) AND (rp.gesamtpreis IS NOT NULL AND rp.gesamtpreis>0)";
	$sql .= " AND rp.projekt_id=p.projekt_id AND (p.mutterprojekt_id=".(int)$mpid;
	if(strlen($mpvn))
		$sql .= " OR p.vorgangsnummer LIKE '".$mpvn."%'";
	$sql .= ") AND p.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND r.mandanten_id=".$GLOBALS['fxpglobals']['client']." AND rp.mandanten_id=".$GLOBALS['fxpglobals']['client'];
	$tmp=db_values($sql, '*');
//fxDebug($tmp, 'getRechnungsDaten()<br />'.$sql);

	return $tmp;
}

/**
 * ???FUNCTION???
 *
 * @param various &$projekt      - Mandatory parameter: ???PARAMETER???
 * @param various &$projektArray - Mandatory parameter: ???PARAMETER???
 */
function addRechnungsDaten(&$projekt, &$projektArray)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$pid=(int)$projekt['projekt_id'];
	if(!fxIsArray($projektArray[$pid]))
		return;

	$ebpc=0.0;
	$ebmc=0.0;

	$rfe=0.0;	// FP
	$rse=0.0;	// SB/MB
	$rre=0.0;	// RK
	$rme=0.0;	// MT

	$rt=(int)$projekt['ist_sachmittel'];
	$gp=get_double($projekt['gesamtpreis']);
	switch($rt)
	{
		case 50:	// FP=Festpreis (Personen)
			$rfe=$gp;
			$ebpc=$gp;
		break;

		case 51:	// SB=Stundenbais (Personen)
			$rse=$gp;
			$ebpc=$gp;
		break;

		case 52:	// RK=Reisekosten (Personen)
			$rre=$gp;
			$ebpc=$gp;
		break;

		case 53:	// MT=Material (Sofort)
			$rme=$gp;
			$ebmc=$gp;
		break;

		case 54:	// FP=Festpreis (Material)
			$rfe=$gp;
			$ebmc=$gp;
		break;

		case 55:	// SB=Stundenbasis (Material)
			$rse=$gp;
			$ebmc=$gp;
		break;

		case 56:	// MB=Mengenbasis (Material)
			$rse=$gp;
			$ebmc=$gp;
		break;
	}

	do
	{
		$projektArray[$pid]['ebpc'] += $ebpc;
		$projektArray[$pid]['ebmc'] += $ebmc;

		$projektArray[$pid]['rfe']=max(0.0, get_double($projektArray[$pid]['rfe'])+$rfe);
		$projektArray[$pid]['rse']=max(0.0, get_double($projektArray[$pid]['rse'])+$rse);
		$projektArray[$pid]['rre']=max(0.0, get_double($projektArray[$pid]['rre'])+$rre);
		$projektArray[$pid]['rme']=max(0.0, get_double($projektArray[$pid]['rme'])+$rme);

		$projektArray[$pid]['rfeo']=max(0.0, get_double($projektArray[$pid]['rfeo'])-$rfe);
		$projektArray[$pid]['rseo']=max(0.0, get_double($projektArray[$pid]['rseo'])-$rse);
		$projektArray[$pid]['rreo']=max(0.0, get_double($projektArray[$pid]['rreo'])-$rre);
		$projektArray[$pid]['rmeo']=max(0.0, get_double($projektArray[$pid]['rmeo'])-$rme);

		$pid=$projektArray[$pid]['e'];
	} while($pid && fxIsArray($projektArray[$pid]));
}

/**
 * ???FUNCTION???
 *
 * @param various $pinf - Mandatory parameter: ???PARAMETER???
 * @param various $iea  - Mandatory parameter: ???PARAMETER???
 * @param various $barr - Mandatory parameter: ???PARAMETER???
 * @param various $rarr - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getProjectDifferences($pinf, $iea, $barr, $rarr)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(fxIsArray($pinf))
	{
		$pc=array();
		$bc=array();
		$zc=array();
		foreach($pinf as $pid => $p)
		{
			// Projektdaten
			// ... Nr. (Vorgangsnummer)
			$db_vn=$p['p']['vorgangsnummer'];
			$dc_vn=$p['vk'];
			if($db_vn != $dc_vn)
				$pc[$pid]['vorgangsnummer']="'".$dc_vn."'";
			if($p['m'] != $p['p']['mutterprojekt_id'])
				$pc[$pid]['mutterprojekt_id']=$p['m'];
			if($p['e'] != $p['p']['elter'])
				$pc[$pid]['elter']=$p['e'];

			$db_pn=$p['p']['name_projekt'];
			$dc_pn=$p['pn'];
			if($db_pn != $dc_pn)
				$pc[$pid]['name_projekt']="'".convert_string($dc_pn,'todb')."'";

			if($p['ib'] != $p['z']['ist_beginn'])
			{
				if(!strlen($p['ib']))
					$zc[$pid]['ist_beginn']='NULL';
				else
					$zc[$pid]['ist_beginn']="'".$p['ib']."'";
			}
			if($p['ie'] != $p['z']['ist_ende'])
			{
				if(!strlen($p['ie']))
					$zc[$pid]['ist_ende']='NULL';
				else
					$zc[$pid]['ist_ende']="'".$p['ie']."'";
			}
			if($p['sb'] != $p['z']['soll_beginn'])
			{
				if(!strlen($p['sb']))
					$zc[$pid]['soll_beginn']='NULL';
				else
					$zc[$pid]['soll_beginn']="'".$p['sb']."'";
			}
			if($p['se'] != $p['z']['soll_ende'])
			{
				if(!strlen($p['se']))
					$zc[$pid]['soll_ende']='NULL';
				else
					$zc[$pid]['soll_ende']="'".$p['se']."'";
			}

			// Aufwände
			// Aufw. Soll (Aufwand_Soll)
			$cv=get_entry(get_double($p['as']) / 3600.0, 'dezimal');
			$dv=get_entry(get_double($p['p']['aufwand_soll']) / 3600.0, 'dezimal');
			if(get_double($cv) != get_double($dv))
				$pc[$pid]['aufwand_soll']=max(0.0, round($p['as'], 4));

			// Aufw. Ist (Aufwand_Ist)
			$cv=get_entry(get_double($p['ai']) / 3600.0, 'dezimal');
			$dv=get_entry(get_double($p['p']['aufwand_ist']) / 3600.0, 'dezimal');
			if(get_double($cv) != get_double($dv))
				$pc[$pid]['aufwand_ist']=max(0.0, round($p['ai'], 4));

			// Abw. (Ist_Verzoeg)
			$cv=get_entry(get_double($p['iv']) / 3600.0, 'dezimal');
			$dv=get_entry(get_double($p['z']['ist_verzoeg']) / 3600.0, 'dezimal');
			if(get_double($cv) != get_double($dv))
				$zc[$pid]['ist_verzoeg']=max(0.0, round($p['iv'], 4));

			// Budget
			// ... Rahmen Geplant (budgetrahmen_pers_int / budgetrahmen_pers_ext)
			foreach($iea as $ie)
			{
				$cvp=get_entry(get_double($p[$ief.'bpr']), 'dezimal');
				$dvp=get_entry(get_double($p['b']['budgetrahmen_pers_'.$ie]), 'dezimal');
				if(get_double($cvp) > get_double($dvp))
					$bc[$pid]['budgetrahmen_pers_'.$ie]=max(0.0, round($p[$ief.'bpr'], 4));

				$cvm=get_entry(get_double($p[$ief.'bmr']), 'dezimal');
				$dvm=get_entry(get_double($p['b']['budgetrahmen_mat_'.$ie]), 'dezimal');
				if(get_double($cvm) > get_double($dvm))
					$bc[$pid]['budgetrahmen_mat_'.$ie]=max(0.0, round($p[$ief.'bmr'], 4));
			}

			// ... Rest. Budgetfelder
			foreach($barr as $bk => $bt)
			{
				// Material-Budget-Felder überspringen ...
				if(substr($bk, 2, 1) == 'm')
					continue;

				$cvp=get_entry(get_double($p[$bk]), 'dezimal');
				$dvp=get_entry(get_double($p['b'][$bt]), 'dezimal');
				if(get_double($cvp) != get_double($dvp))
					$bc[$pid][$bt]=max(0.0, round($p[$bk], 4));

				// ... weil sie aus Personen-Budget-Feldern "erzeugt" werden
				$bk=substr($bk, 0, 2).'m'.substr($bk, 3);
				$bt=str_replace('_pers_', '_mat_', $bt);

				$cvm=get_entry(get_double($p[$bk]), 'dezimal');
				$dvm=get_entry(get_double($p['b'][$bt]), 'dezimal');
				if(get_double($cvm) != get_double($dvm))
					$bc[$pid][$bt]=max(0.0, round($p[$bk], 4));
			}

			// ... Rechnungsfelder
			foreach($rarr as $rk => $rt)
			{
				if(substr($rk, -4) == '_off')
					continue;

				$cva=get_entry(get_double($p[$rk]), 'dezimal');
				$dva=get_entry(get_double($p['b'][$rt]), 'dezimal');
				if(get_double($cva) != get_double($dva))
					$bc[$pid][$rt]=max(0.0, round($p[$rk], 4));

				$rk .= 'o';
				$rt .= '_off';

				$cvo=get_entry(get_double($p[$rk]), 'dezimal');
				$dvo=get_entry(get_double($p['b'][$rt]), 'dezimal');
				if(get_double($cvo) != get_double($dvo))
					$bc[$pid][$rt]=max(0.0, round($p[$rk], 4));
			}
		}
	}
	return array('pc' => $pc, 'zc' => $zc, 'bc' => $bc);
}
?>