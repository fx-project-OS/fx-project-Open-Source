<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : prj_kategorie_pre.inc                                        //
// Version     : 21.2                                                         //
// Begin       : 2020-10-14                                                   //
// Last Change : 2020-10-14                                                   //
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
 * PF 11: Project Categories - Mask Values
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

setForm(true);

// Projektkategorien den Gruppen zuordnen
pk_assignCategories2Groups($err_mode);
fxDebug($GLOBALS['pg'], 'pg', 3);

$cspace=6;
$cpadd=10;
$cheight=26;

$mwidth=(int)db_value("SELECT breite FROM masken WHERE nummer=11 AND mandanten_id=0");
if(!$mwidth)
	$mwidth=(int)db_value("SELECT einstellungswert FROM s_einstellungswerte WHERE einstellung_id=22 AND mandanten_id=0");
$cwidth=floor(($mwidth-$cpadd*4-$cspace)/2);	// 448;
//echo('$cwidth='.$cwidth.'<br>');

// Tooltips
$tt=explode('|', meldung(2781, true));
$tt_gadd=$tt[0];	// Neue Projektkategoriegruppe erstellen
$tt_gdel=$tt[1];	// Diese Projektkategoriegruppe inkl. aller zugeordneten Kategorien löschen (falls möglich)
$tt_gsrt=$tt[2];	// Alle zugeordneten Kategorien dieser Projektkategoriegruppe neu sortieren

$tt=explode('|', meldung(2782, true));
$tt_cmov=$tt[0];	// Diese Projektkategorie zu einer anderen Gruppe zuordnen
$tt_cadd=$tt[1];	// Neue Projektkategorie in dieser Gruppe erstellen
$tt_cdel=$tt[2];	// Diese Projektkategorie löschen

$tt=explode('|', meldung(2783, true));
$tt_fdel=$tt[0];	// Diese Projektkategorie(gruppe) ist eine Standardkategorie(gruppe) von fx-project und kann NICHT gelöscht werden!
$tt_fuse=$tt[1];	// Diese Projektkategorie(gruppe) ist bereits mindestens einem Vorgang zugeordnet und darf deshalb NICHT gelöscht werden!

$tt=explode('|', meldung(3006, true));
$tt_gcol=$tt[0];	// Kategoriegruppenfarbe zuweise
$tt_ccol=$tt[1];	// Kategoriefarbe zuweise

$def_ctop=36;

// Überschrift
$lit_pgroup=get_text(10, '20');	// Prj.-Kategoriegruppe
$lit_pcat=get_text(1, '20');	// Projektkategorien
$lit_sort=get_text(1004, '20');	// Sortiernummer
$lit_name=get_text(1333, '20');	// Bezeichnung

echo('<div id="tset" style="display:none;">');
echo('cspace=I'.$cspace.'|cpadd=I'.$cpadd.'|cheight=I'.$cheight.'|cwidth=I'.$cwidth.'|ctop=I'.$def_ctop);
echo('|maxlength=I'.$val_maxlength);
echo('|form=S'.fxReplaceSpecialCharacters($GLOBALS['_form'],false));
echo('|imgpath=S'.fxReplaceSpecialCharacters($GLOBALS['gfxpath'],false));
echo('|empty=S'.fxReplaceSpecialCharacters($val_empty,false));
echo('|gadd=S'.fxReplaceSpecialCharacters($tt_gadd,false));
echo('|gdel=S'.fxReplaceSpecialCharacters($tt_gdel,false));
echo('|gsrt=S'.fxReplaceSpecialCharacters($tt_gsrt,false));
echo('|cmov=S'.fxReplaceSpecialCharacters($tt_cmov,false));
echo('|cadd=S'.fxReplaceSpecialCharacters($tt_cadd,false));
echo('|cdel=S'.fxReplaceSpecialCharacters($tt_cdel,false));
echo('|fdel=S'.fxReplaceSpecialCharacters($tt_fdel,false));
echo('|fuse=S'.fxReplaceSpecialCharacters($tt_fuse,false));
echo('|gcol=S'.fxReplaceSpecialCharacters($tt_gcol,false));
echo('|ccol=S'.fxReplaceSpecialCharacters($tt_ccol,false));
echo('</div>'.$nl);

$content  = '<div id="tg" style="position:relative;left:0;top:0;">';
$content .= '	<div style="position:relative;left:0px;top:0px;height:28px;">';
$content .= '		<div class="maskh" style="position:absolute;left:0;top:0;width:'.$cwidth.'px;">';
$content .=	'			<b>'.$lit_pgroup.'</b> <i class="s2">('.$lit_sort.'/'.$lit_name.')</i>';
$content .= '		</div>';
$content .= '		<div class="maskh" style="position:absolute;left:'.($cwidth+$cpadd*2+$cspace).'px;top:0;width:'.$cwidth.'px;">';
$content .=	'			<b>'.$lit_pcat.'</b> <i class="s2">('.$lit_sort.'/'.$lit_name.')</i>';
$content .= '		</div>';
$content .= '	</div>';

$gc=1;
foreach($pg as $gunique_id => $g)
{
	// Projektkategoriegruppe
	$gheight=ceil($cheight/2)+(2+sizeof($g['cat']))*$cheight;

	$ct=' '.fxf_jsFunction('prjcatGroupDel', $gc).' tooltip="'.$tt_gdel.'"';
	$ff=' '.fxf_jsFunction('prjcatChangeBG', $gc, -1);
	$ft=$tt_gcol;
	$bg=$g['bg'];
	$cursor='pointer';
	$style='';
	$disabled='';
	if($g['mid'] == 0)
	{
		$ct=' tooltip="'.$tt_fdel.'"';
		$ff='';
		$ft='';
		$bg='transparent';
		$cursor='help';
		$style='opacity:0.5;';
		$disabled='yes';
	}

	$ci=0;

	$content .= '	<div id="t'.$gc.'" style="position:relative;left:0;top:12px;width:'.($cwidth*2+4*$cpadd+$cspace).'px;height:'.$gheight.'px;margin-bottom:8px;box-shadow:2px 2px 6px rgba(0,0,0,0.10);">';

	$content .= '		<div id="g'.$gc.'" style="position:absolute;left:2px;top:8px;width:'.($cwidth+2*$cpadd).'px;height:'.$cheight.'px;z-index:128;">';
	$content .= '			'.fieldText('gp'.$gc, $g['pos'], 45, 0, 'left', false, $disabled);
	$content .= '			'.fieldText('gg'.$gc, $g['col'], 10, 0, 'center', false, $disabled, false, $ft, $ff, 'text', '', 'background:'.$bg.';');
	$content .= '			'.fieldText('gv'.$gc, str_replace('"', '&quot;', $g['val']), ($cwidth-64), $val_maxlength, 'left', false, $disabled);
	$content .= '			'.fieldHidden('gi'.$gc,$g['id']);
	$content .= '			'.fieldHidden('gm'.$gc,$g['mid']);
	if($g['id'] == 2779)
		$content .= '		'.fieldHidden('gua',$gc);
	$content .= '			<div id="gs'.$gc.'" style="position:absolute;right:30px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('.$GLOBALS['gfxpath'].'icso.png);" '.fxf_jsFunction('prjcatRedraw', $gc, 0).' tooltip="'.$tt_gsrt.'"></div>';
	$content .= '			<div id="gd'.$gc.'" style="position:absolute;right:10px;top:4px;width:17px;height:16px;cursor:'.$cursor.';background-image:url('.$GLOBALS['gfxpath'].'icde.png);'.$style.'"'.$ct.'></div>';
	$content .= '		</div>';

	$content .= '		<div id="c'.$gc.'" style="position:relative;left:0;top:0;">';

	// Projektkategorie(n)
	$ctop=$def_ctop;
	$cc=1;
	if(sizeof($g['cat']))
	{
		foreach($g['cat'] as $cunique_id => $c)
		{
			$ct=' '.fxf_jsFunction('prjcatDel', $gc, $cc).' tooltip="'.$tt_cdel.'"';
			$ff=' '.fxf_jsFunction('prjcatChangeBG', $gc, $cc);
			$ft=$tt_ccol;
			$bg=$c['bg'];
			$cursor='pointer';
			$style='';
			$disabled='';
			if(($c['mid'] == 0) || ($c['prj'] > 0))
			{
				$ct=' tooltip="'.$tt_fuse.'"';
				$ff='';
				$ft='';
				$cursor='help';
				$style='opacity:0.5;';
				if($c['mid'] == 0)
				{
					$bg='transparent';
					$ct=' tooltip="'.$tt_fdel.'"';
					$disabled='yes';
				}
			}

			$content .= '			<div id="c'.$gc.'_'.$cc.'" style="position:relative;left:0;top:0;">';
			$content .= '				<div id="cb'.$gc.'_'.$cc.'" style="position:absolute;left:0;top:'.$ctop.'px;width:'.$cwidth.'px;height:'.$cheight.'px;"></div>';
			$content .= '				<div id="cl'.$gc.'_'.$cc.'" style="position:absolute;left:'.(($cwidth+2*$cpadd)/2).'px;top:'.($ctop-12).'px;width:'.(($cwidth+2*$cpadd)/2-22).'px;height:'.$cheight.'px;border-left:1px dotted #bbbbbb;border-bottom:1px dotted #bbbbbb;z-index:64;"></div>';
			$content .= '				<div id="cc'.$gc.'_'.$cc.'" style="position:absolute;left:'.($cwidth+2*$cpadd+$cspace).'px;top:'.$ctop.'px;">';
			$content .= '					<div id="cr'.$gc.'_'.$cc.'" style="position:absolute;left:-22px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('.$GLOBALS['gfxpath'].'icmo.png);" '.fxf_jsFunction('prjcatMove', $gc, $cc, 'event').' tooltip="'.$tt_cmov.'"></div>';
			$content .= '					<div id="ca'.$gc.'_'.$cc.'" style="position:absolute;left:0;top:0;width:'.($cwidth+2*$cpadd).'px;">';
			$content .= '						'.fieldText('cp'.$gc.'_'.$cc, $c['pos'], 45, 0, 'left', false, $disabled);
			$content .= '						'.fieldText('cg'.$gc.'_'.$cc, $c['col'], 10, 0, 'center', false, $disabled, false, $ft, $ff, 'text', '', 'background:'.$bg.';');
			$content .= '						'.fieldText('cv'.$gc.'_'.$cc, str_replace('"', '&quot;', $c['val']), ($cwidth-64), $val_maxlength, 'left', false, $disabled);
			$content .= '						'.fieldHidden('ci'.$gc.'_'.$cc,$c['id']);
			$content .= '						'.fieldHidden('cm'.$gc.'_'.$cc,$c['mid']);
			$content .= '						'.fieldHidden('cu'.$gc.'_'.$cc,$c['prj']);
			$content .= '						<div id="cd'.$gc.'_'.$cc.'" style="position:absolute;right:10px;top:4px;width:17px;height:16px;cursor:'.$cursor.';background-image:url('.$GLOBALS['gfxpath'].'icde.png);'.$style.'"'.$ct.'></div>';
			$content .= '					</div>';
			$content .= '				</div>';
			$content .= '			</div>';

			$ctop += $cheight;
			$cc++;
		}
	}

	$content .= '			<div id="c'.$gc.'_a" style="position:relative;left:0;top:0;">';
	$content .= '				<div id="cb'.$gc.'_a" style="position:absolute;left:0;top:'.$ctop.'px;width:'.$cwidth.'px;height:'.$cheight.'px;"></div>';
	$content .= '				<div id="cl'.$gc.'_a" style="position:absolute;left:'.(($cwidth+2*$cpadd)/2).'px;top:'.($ctop-12).'px;width:'.(($cwidth+2*$cpadd)/2-22).'px;height:'.$cheight.'px;border-left:1px dotted #bbbbbb;border-bottom:1px dotted #bbbbbb;z-index:64;"></div>';
	$content .= '				<div id="cc'.$gc.'_a" style="position:absolute;left:'.($cwidth+2*$cpadd+$cspace).'px;top:'.$ctop.'px;">';
	$content .= '					<div id="ca'.$gc.'_a" style="position:absolute;left:-22px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('.$GLOBALS['gfxpath'].'icad.png);" '.fxf_jsFunction('prjcatAdd', $gc).' tooltip="'.$tt_cadd.'"></div>';
	$content .= '				</div>';
	$content .= '			</div>';

	$content .= '		</div>';

	$content .= '	</div>';

	$gc++;
}
$content .= '</div>';

// Neue Gruppe
$content .= '<div style="position:relative;left:0;top:12px;width:'.($cwidth*2+4*$cpadd+$cspace).'px;height:'.$cheight.'px;margin-bottom:8px;box-shadow:2px 2px 6px rgba(0,0,0,0.10);">';
$content .= '	<div id="gnew" style="position:absolute;left:4px;top:4px;width:17px;height:16px;cursor:pointer;background-image:url('.$GLOBALS['gfxpath'].'icad.png);" '.fxf_jsFunction('prjcatGroupAdd').' tooltip="'.$tt_gadd.'"></div>';
$content .= '	<span style="position:absolute;left:26px;top:4px;">'.$tt_gadd.'</span>';
$content .= '</div><br />';

$maskenfeldwerte['nv_text1']='~'.$content;

echo(maske(11));

$r_msk_nv=true;
?>