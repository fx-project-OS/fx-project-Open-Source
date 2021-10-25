<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : nachschlage_liste_pre.inc                                    //
// Version     : 21.2                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2021-04-01                                                   //
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
 * PF 58: Reference Values - File for presetting the mask fields
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Mask prepearation variables
$index=true;
$mc=$GLOBALS['_maskcounter'];
$pc=0;

$GLOBALS['fxptdata']['lvalues'][$mc]=array();
$GLOBALS['fxptdata']['litf_172']['Checkbox']=get_text(118,'20');	// Delete (Display as headline text in checkbox popup)

// Spaltenbreite(n)
$sql="SELECT zeile, spalte, anzeigegroesse1, spaltenanzahl FROM maskenfelder WHERE nummer=172 AND eingabefeld=149 AND zeile>1";	// TabWert
$sbt=db_values($sql);
//fxDebug($sbt,$sql, 3);
$sql="SELECT zeile, spalte, anzeigegroesse1, spaltenanzahl FROM maskenfelder WHERE nummer=172 AND eingabefeld=1069 AND zeile>1";	// MwSt
$sbm=db_values($sql);
//fxDebug($sbm,$sql, 3);

// MwSt.-Satz-Feld ausblenden, ausser es sollen Einzelbelegsarten geändert werden
$ifl='';
if($eba)
{
	$ifl='1951,1952,1953,1954,1966,1980,2213,2214';	// Kilometerpauschalen und Tagesangabe nicht anzeigen
	$mwst_unit='%';

	if(fxIsArray($sbt) && fxIsArray($sbm))
	{
		$sbd=abs($sbm['anzeigegroesse1'])+30;
		$GLOBALS['TabWert_width']=abs($sbt['anzeigegroesse1'])-$sbd;
		$GLOBALS['column'.$sbt['spalte'].'_twidth']=abs($sbt['spaltenanzahl'])-$sbd;
	}
}
else
{
	$MwSt_Satz_nv=true;
	$MwSt_nv=true;
}

if($ID_Feld)
{
	// Maskenüberschrift
	$mh=get_text(172,'kurz',0,2).':&nbsp;&nbsp;&quot;'.get_text($ID_Feld,'20',0,3).'&quot;';	// Nachschlagewerte (= Original-Maskenüberschrift aus Tabelle "Texte")
	$maske172_head=$mh;

	// Vorhandene Felder anzeigen
	$i=0;
	$arr=array();

	$sql  = "SELECT mandanten_id AS fx_mandanten_id, mandanten_id, wertetabid, position, tabwert, '' AS mwst";
	$sql .= " FROM wertetab";
	$sql .= " WHERE id_sprache=".$Sprache." AND id_feld=".$ID_Feld." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	if(strlen($ifl))
		$sql .= " AND wertetabid NOT IN (".$ifl.")";
	$sql .= " ORDER BY position, tabwert";
	$ta=db_values($sql, '*');
//fxDebug($ta,$sql, 3);
	if(fxIsArray($ta))
	{
		foreach($ta as $tk => $a)
		{
			$wid=(int)$a['wertetabid'];
			$dfm=false;
			if((fxIsArray($arr[$wid]) && !$arr[$wid]['fx_mandanten_id']) || !$a['fx_mandanten_id'] || !$a['mandanten_id'])
				$dfm=true;
			if(!isset($arr[$wid]) || $a['mandanten_id'])
				$arr[$wid]=$a;
			if($dfm)
				$arr[$wid]['fx_mandanten_id']=0;

			if($eba)	// Einzelbelegsarten
			{
				$mp=strpos($arr[$wid]['tabwert'],'^');
				if($mp !== false)
				{
					$arr[$wid]['mwst']=substr($arr[$wid]['tabwert'],$mp+1);
					$arr[$wid]['tabwert']=substr($arr[$wid]['tabwert'],0,$mp);
				}
				else
					$arr[$wid]['mwst']='';
			}
		}
	}
//fxDebug($arr,$sql, 3);

	if(fxIsArray($arr))
	{
		fxpGetCSSColors();

		$wta=explode('|',meldung(3341,false));	// Standardwert|Abgeänderter Standardwert|Benutzerwert
		$cvm=meldung(3342,false);				// Dies ist ein <b>Abgeänderter Standardwert</b>.<br /><br /><i class=grey>(Wenn Sie diesen Wert löschen, wird er durch den ursprünglichen Standardwert ersetzt!)</i>
		$spacer='<img src="'.$GLOBALS['gfxpath'].'dmy.png" style="height:1px;width:3px;">';

		// Line height should be 24px
		$Position_height=24;

		// No right border in the following columns
		fxf_maskColumnBorder(array('Checkbox'), 0, -1, $mc);

		foreach($arr as $wid => $a)
		{
			$changeable=true;
			// Standardwert
			if(!$a['mandanten_id'])
			{
				if(!$eba || ($aktion == FXP_DELETE))
					$changeable=false;
				$cls_add=' darkgrey';
				$symbol='&uarr;';
				$tooltip='<b class='.trim($cls_add).'>'.$symbol.'</b>&nbsp;&nbsp;<span class='.trim($cls_add).'>'.$wta[0].'</span>';
			}
			// Abgeänderter Standardwert
			else if(!$a['fx_mandanten_id'])
			{
				$cls_add=' blue';
				$symbol='&darr;';
				$tooltip='<b class='.trim($cls_add).'>'.$symbol.'</b>&nbsp;&nbsp;<span class='.trim($cls_add).'>'.$wta[1].'</span>';
			}
			// Benutzerwert
			else
			{
				$cls_add=' black';
				$symbol='&nbsp;';
				$tooltip='<b class='.trim($cls_add).'>'.$symbol.'</b>&nbsp;&nbsp;<span class='.trim($cls_add).'>'.$wta[2].'</span>';
			}
			$col=trim($cls_add);

			$deleteable=$changeable;
			$deletemsg='';
			$changemsg='';
			if(($aktion == FXP_DELETE) && $deleteable)
			{
				$deletemsg=checkClientLookupValueUsage($wid, $ID_Feld);
				if(strlen($deletemsg))
				{
					$deleteable=false;
					$cls_add=' lightred';
				}
				else if(!$a['fx_mandanten_id'])
					$changemsg=$cvm;
//echo('<b>'.$wid.'</b> ('.$ID_Feld.'):<br />'.$deletemsg.'<hr />');
			}

			$clsr='^class_rv'.(($i+1)%2).$cls_add.'|';
			$clst='^class_transparent'.$cls_add.'|';

			if($aktion != FXP_CHANGE)
			{
				// ...(Checkbox)
				if($aktion == FXP_DELETE)
				{
					if($deleteable)
					{
						// Default- oder Fehlerwerte?
						$vc='';
						if($err)
							$vc=$_POST['Checkbox'][$mc][$i];
						$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']=$vc;
						$pc++;
					}
					else if($changeable)
					{
						$GLOBALS['Checkbox_dis['.$mc.']['.$i.']']=true;
						$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='';
					}
					else
						$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~';
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~#'.$tooltip.'|^class_'.$col.'|~'.$symbol.substr('00'.($i+1), -2).':&nbsp;';
				// ...Position
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']=$clsr.'~'.$a['position'].$spacer;
				// ...Wert
				$tt='';
				if(strlen($deletemsg))
					$tt='~#'.$deletemsg.'|';
				else if(strlen($changemsg))
					$tt='~#'.$changemsg.'|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']=$tt.$clsr.'~'.$spacer.strtr($a['tabwert'], $tabwert_strtr);
				if(strlen($deletemsg) || strlen($changemsg))
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert'] .= '<img src="'.$GLOBALS['gfxpath'].'wrn_b_13x13.png" width="13" height="13" style="float:right;margin-right:2px;opacity:0.5;">';
				else if($aktion == FXP_DISPLAY)
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert'] .= '<div class="s2 italic ainactive'.$cls_add.'" style="float:right;">(ID: '.$a['wertetabid'].')</div>';
				// ...MwSt.
				if($eba)
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['mwst']=$clsr.'~'.getVATRate($a['mwst'], 'decimal').$spacer;
			}
			else
			{
				// Default- oder Fehlerwerte?
				$vp=$a['position'];
				$vt=$a['tabwert'];
				$vm='';
				if($eba)
					$vm=getVATRate($a['mwst'], 'input');
				if($err && $changeable)
				{
					$vp=$_POST['Position'][$mc][$i];
					$vt=$_POST['TabWert'][$mc][$i];
					if($eba)
						$vm=getVATRate($_POST['MwSt'][$mc][$i], 'input');
				}

				// ...(Checkbox)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~#'.$tooltip.'|'.$clst.'~'.$symbol.$spacer;
				// ...Position
				if($changeable)
				{
					if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
						$GLOBALS['Position_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']=$clst.$vp;
					$pc++;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']=$clsr.'~'.$vp.$spacer;
				// ...Wert
				if($changeable)
				{
					if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
						$GLOBALS['TabWert_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']=$clst.$vt;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']=$clsr.'~'.$spacer.strtr($a['tabwert'], $tabwert_strtr);
				// ...MwSt.
				if($eba)
				{
					if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
						$GLOBALS['MwSt_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['mwst']='~#'.$msm.'|'.$clst.$vm;
				}
			}

			// ...[Hidden]
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['nv_text1']=$a['fx_mandanten_id'].'<|>'.$a['mandanten_id'].'<|>'.$wid.'<|>'.$a['position'].'<|>'.$a['tabwert'].'<|>'.$a['mwst'];

			$i++;
		}
	}

	// Empty line with top border?
	if($i)
	{
		fxf_maskColumnBorder(array('Checkbox|0', 'Position', 'TabWert', 'MwSt'), 1, -1, $mc, $i, true);
		$i++;
	}

	if($aktion == FXP_CREATE)	// Leere Zeile einfügen
	{
		for($j=0; $j<$GLOBALS['fxpglobals']['settings']['listenneu']; $j++)
		{
			// Default- oder Fehlerwerte?
			$vp=0;
			$vt='';
			$vm='';
			if($err)
			{
				$vp=$_POST['Position'][$mc][$i];
				$vt=$_POST['TabWert'][$mc][$i];
				if($eba)
					$vm=getVATRate($_POST['MwSt'][$mc][$i], 'input');
			}

			// ...(Checkbox)
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~<b>+</b>';
			// ...Position
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']='^class_transparent|'.$vp;
			// ...Wert
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']='^class_transparent|'.$vt;
			// ...MwSt.
			if($eba)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['mwst']='~#'.$msm.'|^class_transparent|'.$vm;
			// ...[Hidden]
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['nv_text1']=$GLOBALS['fxpglobals']['client'].'<|>'.$GLOBALS['fxpglobals']['client'].'<|>0<|>'.$vp.'<|>'.$vt.'<|>'.$vm;

			// No borders
			fxf_maskColumnBorder(array('Checkbox', 'Position', 'TabWert', 'MwSt'), 0, -1, $mc, $i);

			$i++;
			$pc++;
		}
	}
}

// Button ausblenden?
if(!$pc)
	$Button_Speichern_nv=true;
if($aktion == FXP_DELETE)
	$Checkbox_sf=true;
?>