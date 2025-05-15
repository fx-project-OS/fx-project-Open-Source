<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : nachschlage_liste_pre.inc                                    //
// Version     : 24.1                                                         //
// Begin       : 2020-08-27                                                   //
// Last Change : 2025-01-30                                                   //
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
 * @version 24.1
 */

// Mask prepearation variables
$index=true;
$mc=$GLOBALS['_maskcounter'];
$pc=0;

$GLOBALS['fxptdata']['lvalues'][$mc]=array();
$GLOBALS['fxptdata']['litf_172']['Checkbox']=get_text(118,'20');	// Delete (Display as headline text in checkbox popup)

// Spaltenbreite(n): TabWert=149, MwSt=1069, CodeWert=2085 + icon=426;
$sql="SELECT spalte, eingabefeld, anzeigegroesse1, spaltenanzahl FROM maskenfelder WHERE nummer=172 AND zeile>1";
$tmp=db_values($sql, '*');
//fxDebug($tmp,$sql, 0);
$sba=array();
foreach($tmp as $t)
	$sba[$t['eingabefeld']]=array('sp'=>$t['spalte'], 'ag'=>$t['anzeigegroesse1'], 'sa'=>$t['spaltenanzahl']);
//fxDebug($sba,$sql, 0);

// Factur-X Codes: 63=Länderkennzeichen, 75=Währung + 1054=Verkaufseinheit
$fca=array(63=>false, 75=>false, 1054=>false);

// MwSt., Code + icon ausblenden, ausser es sollen Einzelbelegsarten oder Factur-X Werte geändert werden
$ifl='';
if($eba)	// Einzelbelegsart
{
	$ifl='1951,1952,1953,1954,1966,1980,2213,2214,3403,3405';	// Kilometerpauschalen und Tagesangabe nicht anzeigen
	$mwst_unit='%';

	$sbd=abs($sba[1069]['ag'])+21;
	$GLOBALS['TabWert_width']=abs($sba[149]['ag'])-$sbd;
	$GLOBALS['column'.$sba[149]['sp'].'_twidth']=abs($sba[149]['sa'])-$sbd;

	$CodeWert_nv=true;
	$icon_nv=true;
}
else if($fcx)	// Factur-X: Länderkennzeichen, Währung oder Verkaufseinheit
{
//	$codewert_unit='<div style="position:absolute;right:4px;background:#fff;border:1px solid #000;font-size:12pt;padding:4px 8px;">?</div>';

	$sbd=abs($sba[2085]['ag'])+abs($sba[426]['ag'])+16;
	$GLOBALS['TabWert_width']=abs($sba[149]['ag'])-$sbd;
	$GLOBALS['column'.$sba[149]['sp'].'_twidth']=abs($sba[149]['sa'])-$sbd;

	$MwSt_Satz_nv=true;
	$MwSt_nv=true;

	// We need our Factur-X class in order to get default codes for 63=Länderkennzeichen, 75=Währung + 1054=Verkaufseinheit
	if(!class_exists('FACTURX'))
	{
		$req_fn=fxf_fn_reqFilename('facturx');
//echo('require: '.$req_fn.'<br />');
		if(strlen((string)$req_fn))
			require($req_fn);
	}
	if(class_exists('FACTURX'))
	{
		$fcc=new FACTURX();
		$fca[63]=$fcc->fxcCountryID();
		$fca[75]=$fcc->fxcCurrencyID();
		$fca[1054]=$fcc->fxcUnitCode();
//fxDebug($fca,'$fca: Factur-X default codes', 0);

		$close_literal=meldung(968,true);

		// Create select fields
		foreach($fca as $fck => $fa)
		{
			$tfn='fcx'.$fck;
			$GLOBALS[$tfn.'select']=$fa;
			$GLOBALS[$tfn.'_ds']=794;	// -empty-
//fxDebug($GLOBALS[$tfn.'select'],$tfn, 0);

			echo('<div id="sel'.$fck.'" class="maskc" style="position:absolute;left:0;top:0;padding:8px;border-left:0;border-radius:0;border-top-right-radius:8px;border-bottom-right-radius:8px;text-align:left;z-index:128;display:none;">Factur-X Code:&nbsp;&nbsp;&quot;<b>'.get_text($fck,'20',0,3).'</b>&quot;');
			echo('<img class="sic_close" src="'.$GLOBALS['gfxpath'].'ic_cls_b_36x24.png" tooltip="'.$close_literal.'" onclick="fxf_fn_closeElement58();" onmouseover="$(this).src=\''.$GLOBALS['gfxpath'].'ic_cls_w_36x24.png\';" onmouseout="$(this).src=\''.$GLOBALS['gfxpath'].'ic_cls_b_36x24.png\';" style="position:absolute;right:0;top:0;width:36px;height:24px;cursor:pointer;">'.$GLOBALS['nl']);
			echo('<br /><br />'.fieldSelect($tfn, '', 400));
			echo('<div style="position:absolute;left:0;top:49%;width:4px;height:4px;background:#000;"></div>');
			echo('</div>'.$GLOBALS['nl']);
		}
	}
}
else
{
	$MwSt_Satz_nv=true;
	$MwSt_nv=true;
	$CodeWert_nv=true;
	$icon_nv=true;
}

if($ID_Feld)
{
	// Maskenüberschrift
	$mh=get_text(172,'kurz',0,2).':&nbsp;&nbsp;&quot;'.get_text($ID_Feld,'20',0,3).'&quot;';	// Nachschlagewerte (= Original-Maskenüberschrift aus Tabelle "Texte")
	$maske172_head=$mh;

	// Vorhandene Felder anzeigen
	$i=0;
	$arr=array();

	$sql  = "SELECT mandanten_id AS fx_mandanten_id, mandanten_id, wertetabid, position, tabwert, codewert, '' AS mwst";
	$sql .= " FROM wertetab";
	$sql .= " WHERE id_sprache=".$Sprache." AND id_feld=".$ID_Feld." AND mandanten_id IN (0,".$GLOBALS['fxpglobals']['client'].")";
	if(strlen((string)$ifl))
		$sql .= " AND wertetabid NOT IN (".$ifl.")";
	$sql .= " ORDER BY position, tabwert";
	$ta=db_values($sql, '*');
//fxDebug($ta,$sql, 0);
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
			$arr[$wid]['codewert']=(string)$a['codewert'];
			$arr[$wid]['mwst']=(string)$a['mwst'];

			if($eba)	// Einzelbelegsart
			{
				$mp=strpos($arr[$wid]['tabwert'],'^');
				if($mp !== false)
				{
					$arr[$wid]['mwst']=substr((string)$arr[$wid]['tabwert'],$mp+1);
					$arr[$wid]['tabwert']=substr((string)$arr[$wid]['tabwert'],0,$mp);
				}
				else
					$arr[$wid]['mwst']='';
			}
		}
	}
//fxDebug($arr,$sql, 0);

	if(fxIsArray($arr))
	{
		fxpGetCSSColors();

		$wta=explode('|',meldung(3341,false));	// Standardwert|Abgeänderter Standardwert|Benutzerwert
		$cvm=meldung(3342,false);				// Dies ist ein <b>Abgeänderter Standardwert</b>.<br /><br /><i class=grey>(Wenn Sie diesen Wert löschen, wird er durch den ursprünglichen Standardwert ersetzt!)</i>
		$fxu=meldung(3425,false);				// Unknown Code <i>(Stand: Factur-X Version 1.07.2 (ZUGFeRD v. 2.3.2) / November 15, 2024)</i>
		$fxe=meldung(3426,false);				// Empty Factur-X Code <i>(A Factur-X invoice cannot be created!)</i>
		$fxs=meldung(3427,false);				// Select Factur-X Code from list
		$spacer='<img src="'.$GLOBALS['gfxpath'].'dmy.png" style="height:1px;width:3px;">';

		// Line height should be 24px
		$Position_height=24;

		// No right border in the following columns
		fxf_maskColumnBorder(array('Checkbox', 'icon'), 0, -1, $mc);

		foreach($arr as $wid => $a)
		{
			$changeable=true;
			if(($aktion == FXP_DISPLAY) || ($aktion == FXP_DELETE))
				$changeable=false;

			$deleteable=true;
			if(!$a['mandanten_id'])
				$deleteable=false;

			// Standardwert
			if(!$a['mandanten_id'])
			{
				$cls_add=' darkgrey';
				$symbol='&uarr;';
				$tooltip='<b class='.trim((string)$cls_add).'>'.$symbol.'</b>&nbsp;&nbsp;<span class='.trim((string)$cls_add).'>'.$wta[0].'</span>';
			}
			// Abgeänderter Standardwert
			else if(!$a['fx_mandanten_id'])
			{
				$cls_add=' blue';
				$symbol='&darr;';
				$tooltip='<b class='.trim((string)$cls_add).'>'.$symbol.'</b>&nbsp;&nbsp;<span class='.trim((string)$cls_add).'>'.$wta[1].'</span>';
			}
			// Benutzerwert
			else
			{
				$cls_add=' black';
				$symbol='&nbsp;';
				$tooltip='<b class='.trim((string)$cls_add).'>'.$symbol.'</b>&nbsp;&nbsp;<span class='.trim((string)$cls_add).'>'.$wta[2].'</span>';
			}
			$col=trim((string)$cls_add);

			$deletemsg='';
			$changemsg='';
			if(($aktion == FXP_DELETE) && $deleteable)
			{
				$deletemsg=checkClientLookupValueUsage($wid, $ID_Feld);
				if(strlen((string)$deletemsg))
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

			// Default- oder Fehlerwerte?
			$vp=$a['position'];
			$vt=$a['tabwert'];
			$vm='';
			if($eba)
				$vm=getVATRate($a['mwst'], 'input');
			$vc='';
			if($fcx)
				$vc=(string)$a['codewert'];
			if(fxIsArray($_POST) && $_POST['Button_Speichern'] && $err && $changeable)
			{
				$vp=$_POST['Position'][$mc][$i];
				$vt=$_POST['TabWert'][$mc][$i];
				if($eba)
					$vm=getVATRate($_POST['MwSt'][$mc][$i], 'input');
				if($fcx)
					$vc=$_POST['CodeWert'][$mc][$i];
			}

			// (Checkbox)
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
			else if($aktion == FXP_CHANGE)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~#'.$tooltip.'|'.$clst.'~'.$symbol.$spacer;
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~#'.$tooltip.'|^class_'.$col.'|~'.$symbol.substr('00'.($i+1), -2).':&nbsp;';

			// Position
			if($aktion == FXP_CHANGE)
			{
				if($changeable)
				{
					if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
						$GLOBALS['Position_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']=$clst.$vp;
					$pc++;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']=$clsr.'~'.$vp.$spacer;
			}
			else
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']=$clsr.'~'.$a['position'].$spacer;

			// Wert
			if($aktion == FXP_CHANGE)
			{
				if($changeable)
				{
					if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
						$GLOBALS['TabWert_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']=$clst.$vt;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']=$clsr.'~'.$spacer.strtr($a['tabwert'], $tabwert_strtr);
			}
			else
			{
				$tt='';
				if(strlen((string)$deletemsg))
					$tt='~#'.$deletemsg.'|';
				else if(strlen((string)$changemsg))
					$tt='~#'.$changemsg.'|';
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']=$tt.$clsr.'~'.$spacer.strtr($a['tabwert'], $tabwert_strtr);
				if(strlen((string)$deletemsg) || strlen((string)$changemsg))
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert'] .= '<img src="'.$GLOBALS['gfxpath'].'wrn_b_13x13.png" width="13" height="13" style="float:right;margin-right:2px;opacity:0.5;">';
				else if($aktion == FXP_DISPLAY)
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert'] .= '<div class="s2 italic ainactive'.$cls_add.'" style="float:right;">(ID: '.$a['wertetabid'].')</div>';
			}

			// MwSt.
			if($eba)
			{
				if($aktion == FXP_CHANGE)
				{
					if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
						$GLOBALS['MwSt_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['mwst']='~#'.$msm.'|'.$clst.$vm;
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['mwst']=$clsr.'~'.getVATRate($a['mwst'], 'decimal').$spacer;
			}

			// Code
			if($fcx)
			{
				if($aktion == FXP_CHANGE)
				{
					if($changeable)
					{
						if(fxIsArray($GLOBALS['cstyle']) && isset($GLOBALS['cstyle'][$col]))
							$GLOBALS['CodeWert_style['.$mc.']['.$i.']']='color:#'.$GLOBALS['cstyle'][$col].';';
						$GLOBALS['fxptdata']['lvalues'][$mc][$i]['codewert']=$clst.$vc;
					}
					else
						$GLOBALS['fxptdata']['lvalues'][$mc][$i]['codewert']=$clsr.'~'.$spacer.strtr($a['codewert'], $tabwert_strtr);
				}
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['codewert']=$clsr.'~'.strtr($a['codewert'], $tabwert_strtr).$spacer;
			}

			// (icon)
			if($fcx)
			{
				$ia=nswCode($a['codewert']);
				if($aktion == FXP_CHANGE)
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon']='~##'.$ia['it'].'<br />'.$fxs.'|^class_'.$col.'" onclick="fxf_fn_openElement58('.$mc.', '.$i.', '.$ID_Feld.', 5);|~<div style="position:relative;">'.$ia['ic'].'<img src="'.$GLOBALS['gfxpath'].'arrpv.png" style="position:absolute;left:13px;" /></div>';
				else
					$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon']='~#'.$ia['it'].'|^class_'.$col.'|~'.$ia['ic'];
			}

			// [Hidden]
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['nv_text1']=$a['fx_mandanten_id'].'<|>'.$a['mandanten_id'].'<|>'.$wid.'<|>'.$a['position'].'<|>'.$a['tabwert'].'<|>'.$a['mwst'].'<|>'.$a['codewert'];

			$i++;
		}
	}

	// Empty line with top border?
	if($i)
	{
		fxf_maskColumnBorder(array('Checkbox|0', 'Position', 'TabWert', 'MwSt', 'CodeWert'), 1, -1, $mc, $i, true);
		$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon']='~|';
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
			$vc='';
			if($err)
			{
				$vp=$_POST['Position'][$mc][$i];
				$vt=$_POST['TabWert'][$mc][$i];
				if($eba)
					$vm=getVATRate($_POST['MwSt'][$mc][$i], 'input');
				if($fcx)
					$vc=$_POST['CodeWert'][$mc][$i];
			}

			// (Checkbox)
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['checkbox']='~<b>+</b>';

			// Position
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['position']='^class_transparent|'.$vp;

			// Wert
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['tabwert']='^class_transparent|'.$vt;

			// MwSt.
			if($eba)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['mwst']='~#'.$msm.'|^class_transparent|'.$vm;

			// Code
			if($fcx)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['codewert']='^class_transparent|'.$vc;

			// (icon)
			if($fcx)
				$GLOBALS['fxptdata']['lvalues'][$mc][$i]['icon']='~##'.$fxs.'|^class_'.$col.'" onclick="fxf_fn_openElement58('.$mc.', '.$i.', '.$ID_Feld.', 0);|~<img src="'.$GLOBALS['gfxpath'].'arrpv.png" />';

			// [Hidden]
			$GLOBALS['fxptdata']['lvalues'][$mc][$i]['nv_text1']=$GLOBALS['fxpglobals']['client'].'<|>'.$GLOBALS['fxpglobals']['client'].'<|>0<|>'.$vp.'<|>'.$vt.'<|>'.$vm.'<|>'.$vc;

			// No borders
			fxf_maskColumnBorder(array('Checkbox', 'Position', 'TabWert', 'MwSt', 'CodeWert', 'icon'), 0, -1, $mc, $i);

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


/**
 * Get tooltip and icon for the Factur-X icon field
 *
 * @param string $code - Mandatory parameter:  Factur-X code
 *
 * @return Array containing tooltip and icon
 */
function nswCode($code)
{
	if(fxIsArray($GLOBALS['fca'][$GLOBALS['ID_Feld']]) && isset($GLOBALS['fca'][$GLOBALS['ID_Feld']][$code]))	// Factur-X Code exists
	{
		$it='Factur-X Code: <b class=green>'.$code.'</b> &rarr; '.$GLOBALS['fca'][$GLOBALS['ID_Feld']][$code];
		$ic='<img src="'.$GLOBALS['gfxpath'].'tl_green.png" />';
	}
	else if(!strlen($code))																						// Leerer Code
	{
		$it='<span class=red>'.$GLOBALS['fxe'].'</span>';
		$ic='<img src="'.$GLOBALS['gfxpath'].'tl_red.png" />';
	}
	else																										// Unbekannter Code
	{
		$it='<span class=grey>'.$GLOBALS['fxu'].'</span>';
		$ic='<img src="'.$GLOBALS['gfxpath'].'tl_yellow.png" />';
	}

	return array('it'=>$it, 'ic'=>$ic);
}
?>