<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_edit_pre.inc                                          //
// Version     : 24.1                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2025-02-18                                                   //
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
 * PF 217: Form Designer - Edit Configuration File
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

setForm(true);

// Configuration file (kdatei)
$kdatei=$_POST['kdatei'];
if(substr((string)$kdatei, 0, 1) == '_')
	$kdatei=substr((string)$kdatei,1);
$ddatei=$kdatei;
$inact=false;
$kcol='black';
if(substr((string)$kdatei,-5,1) == '!')
{
	$inact=true;
	$kcol='red';
	$ddatei=substr((string)$kdatei,0,-5).substr((string)$kdatei,-4);
}
$maskenfeldwerte['kdatei']='~<font class="'.$kcol.'">'.$ddatei.'</font>';
// Icons, if configuration file exists
if(($_POST['mordner'] != '0') && file_exists($kdatei))
{
	$ita=explode('|',meldung(2046,false));	// Deactivate configuration file|Reactivate configuration file|Delete configuration file
	// Reactivate
	if($inact)
		$maskenfeldwerte['kdatei'] .= '<img id="reaconf" src="'.$GLOBALS['gfxpath'].'icrs.png" align="top" tooltip="'.$ita[1].'" style="cursor:pointer;margin-left:12px;" '.fxf_jsFunction('loadTR', $GLOBALS['fxpglobals']['tr'], 'react').'>';	// Konfigurationsdatei reaktivieren
	// Deactivate
	else
		$maskenfeldwerte['kdatei'] .= '<img id="inaconf" src="'.$GLOBALS['gfxpath'].'icri.png" align="top" tooltip="'.$ita[0].'" style="cursor:pointer;margin-left:12px;" '.fxf_jsFunction('loadTR', $GLOBALS['fxpglobals']['tr'], 'inact').'>';	// Konfigurationsdatei deaktivieren
	// Delete
	$maskenfeldwerte['kdatei'] .= '<img id="delconf" src="'.$GLOBALS['gfxpath'].'icde.png" align="top" tooltip="'.$ita[2].'" style="cursor:pointer;margin-left:4px;" '.fxf_jsFunction('configDeleteConf').'>';	// Konfigurationsdatei löschen
}
$maskenfeldwerte['kdatei'] .= '<br /><br />';

// Hidden
$maskenfeldwerte['kdatei'] .= fieldHidden('cpath',$kdatei).fieldHidden('force_save',0);

// Toolbar (kbar)
$istyle=' style="background:#cccccc;border:1px solid #bbbbbb;border-radius:4px;box-shadow:1px 1px 2px #666666;padding:2px;margin:2px;cursor:pointer;"';
$iwidth=120;
$kbar  = '<table border="0" cellpadding="0" cellspacing="0">';
$kbar .= '<tr>';
// ...Upload Picture
$kbar .= '<td><img id="ic_upload" src="'.$GLOBALS['gfxpath'].'cfg_upl.png"'.$istyle.'></td>';
$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[0].'</font></td>';
// ...Integrate Picture
$cs=6;
$kbar .= '<td><img id="ic_integrate" src="'.$GLOBALS['gfxpath'].'cfg_dnl.png"'.$istyle.'></td>';
$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[1].'</font></td>';
if($ktyp != 2018)
{
	// ...Calculator
	$cs -= 2;
	$kbar .= '<td><img id="ic_calculator" src="'.$GLOBALS['gfxpath'].'cfg_cal.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[2].'</font></td>';
	if($ktyp == 2021)
	{
		// ...Color
		$cs -= 2;
		$kbar .= '<td><img id="ic_color" src="'.$GLOBALS['gfxpath'].'cfg_col.png"'.$istyle.'></td>';
		$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[3].'</font></td>';
	}
}
$kbar .= '<td colspan="'.$cs.'">&nbsp;</td>';
$kbar .= '</tr>';
if($ktyp != 2018)
{
	$kbar .= '<tr>';
	// ... Picture
	$kbar .= '<td><img id="icg_pic" src="'.$GLOBALS['gfxpath'].'cfg_pic.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[4].'</font></td>';
	// ... Text
	$kbar .= '<td><img id="icg_txt" src="'.$GLOBALS['gfxpath'].'cfg_txt.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[5].'</font></td>';
	// ... Box
	$kbar .= '<td><img id="icg_box" src="'.$GLOBALS['gfxpath'].'cfg_box.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[6].'</font></td>';
	// ... Rectangle
	$kbar .= '<td><img id="icg_rec" src="'.$GLOBALS['gfxpath'].'cfg_rec.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[7].'</font></td>';
	// ... Line
	$kbar .= '<td><img id="icg_lin" src="'.$GLOBALS['gfxpath'].'cfg_lin.png"'.$istyle.'></td>';
	$kbar .= '<td width="'.$iwidth.'">&nbsp;<font class="s2 grey">'.$text_icons[8].'</font></td>';
	$kbar .= '</tr>';
}
$kbar .= '</table>';
$maskenfeldwerte['kbar']='~'.$kbar;

// Include integration file
$_GET['ktyp']=$ktyp;
$_GET['mordner']=$mordner;
$req_fn=fxf_fn_reqFilename('config_integrate');
if(strlen((string)$req_fn))
	require($req_fn);

// Command line (Textzeile)
$litwidth=134;
// ...Status
$befehl  = '<div id="conf_status_div" style="margin:2px 0;padding:2px 4px;text-align:left;box-shadow:1px 1px 4px #666666;padding:4px;display:block;">';
$befehl .= '	<font color="grey">'.get_text(1843, '10').'</font>';
$befehl .= '</div>';
// ...Area: Upload Picture
$befehl .= '<div id="conf_upload_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
$befehl .= '		<tr>';
$befehl .= '			<td class="maskh" colspan="2" align="top" width="100%">';
$befehl .= '				<b>'.$text_icons[0].'</b>'.closeIcon(0,0, 36, 'cfg_upl_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'upload'));
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '		<tr>';
$befehl .= '			<td width="'.$litwidth.'" valign="top" nowrap><br />&nbsp;&nbsp;<span class="lit">'.get_text(1572, '15').':</span>&nbsp;</td>';	// Quelle (Datei)
$befehl .= '			<td valign="top" nowrap>';
$befehl .= '				<br />'.fieldFile('file',792).'<br /><br /><br />';
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '	</table>';
$befehl .= '</div>';
// ...Area: Integrate Picture
$befehl .= '<div id="conf_integrate_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
$befehl .= '		<tr>';
$befehl .= '			<td class="maskh" align="top" width="100%">';
$befehl .= '				<b>'.$text_icons[1].'</b>'.closeIcon(0,0, 36, 'cfg_dnl_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'integrate'));
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '		<tr>';
$befehl .= '			<td valign="top">';
$befehl .= '				<br /><div id="conf_integrate_pics" style="margin:0 6px;">';
$befehl .= $integrate;
$befehl .= '				</div><br />';
$befehl .= '			</td>';
$befehl .= '		</tr>';
$befehl .= '	</table>';
$befehl .= '</div>';

if($ktyp != 2018)
{
	// ...Area: Calculator
	$befehl .= '<div id="conf_calculator_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
	$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
	$befehl .= '		<tr>';
	$befehl .= '			<td class="maskh" colspan="2" align="top" width="100%">';
	$befehl .= '				<b>'.$text_icons[2].'</b>'.closeIcon(0,0, 36, 'cfg_cal_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'calculator'));
	$befehl .= '			</td>';
	$befehl .= '		</tr>';
	$befehl .= '		<tr>';
	$befehl .= '			<td valign="top" align="left" nowrap>';
	$befehl .= '				<br />&nbsp;&nbsp;<span class="lit">'.$unit_mm.':</span>&nbsp;&nbsp;'.fieldText('conf_unit_mm',get_entry(0, 'dezimal'),125);
	$befehl .= '				&nbsp;x&nbsp;2.83501683501&nbsp;&nbsp;&nbsp;';
	$befehl .= '			</td>';
	$befehl .= '			<td valign="top" align="right" nowrap>';
	$befehl .= '				<br /><span class="lit">'.$unit_pdf.':</span>&nbsp;&nbsp;'.fieldText('conf_unit_pdf', '0.00', 125).'&nbsp;&nbsp;<br /><br />';
	$befehl .= '			</td>';
	$befehl .= '		</tr>';
	$befehl .= '	</table>';
	$befehl .= '</div>';
	if($ktyp == 2021)
	{
		// ...Area: Color
		$befehl .= '<div id="conf_color_div" class="maskc" style="position:relative;text-align:left;margin:8px 0;display:none;">';
		$befehl .= '	<table width="100%" border="0" cellspacing="0" cellpadding="1">';
		$befehl .= '		<tr>';
		$befehl .= '			<td class="maskh" align="top" width="100%">';
		$befehl .= '				<b>'.$text_icons[3].'</b>'.closeIcon(0,0, 36, 'cfg_col_close', 'border-top-right-radius:10px;', fxf_jsFunction('configClose', 'color'));
		$befehl .= '			</td>';
		$befehl .= '		</tr>';
		$befehl .= '		<tr>';
		$befehl .= '			<td valign="top" align="center" nowrap>';
		$befehl .= '				<br /><input id="conf_color_input" type="text" class="color" value="#006B9F"><br />';
		$befehl .= '			</td>';
		$befehl .= '		</tr>';
		$befehl .= '	</table>';
		$befehl .= '</div>';
	}
}
$maskenfeldwerte['textzeile']='~'.$befehl;

// Text area (Text_varLang)
$konfig='';
$intemp='';
$ndatei=$kdatei;
if($err && $_POST['Text_varLang'])
	$konfig=$_POST['Text_varLang'];
if(!strlen((string)$konfig) && file_exists($ndatei))
	$konfig=fxLoad($ndatei);
//fxDebug($konfig,'$konfig [L]: $ndatei='.$ndatei, 0);
if(!strlen((string)$konfig))
{
	$spos=strrpos($kdatei, '/');
	if($_POST['ktyp'] == 2021)		// 2021=Invoice
		$nkdatei=substr((string)$kdatei,$spos,9).'_'.$GLOBALS['fxpglobals']['lang'].'.tmp';
	else if($_POST['ktyp'] == 2019)	// 2019=Performance proof
		$nkdatei=substr((string)$kdatei,$spos,9).substr((string)$kdatei,-4);
	else
		$nkdatei=substr((string)$kdatei,$spos);
	$ndatei=$GLOBALS['datpath'].$_POST['mordner'].$nkdatei;
	if(!file_exists($ndatei))
		$ndatei=$GLOBALS['datpath'].'0'.$nkdatei;
	if(file_exists($ndatei))
	{
		$konfig=fxLoad($ndatei);
		if($_POST['ktyp'] == 2021)		// 2021=Invoice
		{
			$ndatei='';
			$intemp=$konfig;
		}
	}
//fxDebug($konfig,'$konfig [N]: $ndatei='.$ndatei, 0);
}
else if($_POST['ktyp'] == 2021)		// 2021=Invoice
{
	$nkdatei=$GLOBALS['datpath'].'0/cfg_prng_'.$GLOBALS['fxpglobals']['lang'].'.tmp';
	if(file_exists($nkdatei))
		$intemp=fxLoad($nkdatei);
//fxDebug($intemp,'$intemp: $nkdatei='.$nkdatei, 0);
}

// New?
if(substr((string)$_POST['kdatei'], 0, 1) == '_')
{
	$bpos=strpos($konfig,'// Begin       : ');
	if($bpos !== false)
		$konfig=substr((string)$konfig,0,$bpos+17).$today.substr((string)$konfig,$bpos+27);
	$cpos=strpos($konfig,'// Last Change : ');
	if($cpos !== false)
		$konfig=substr((string)$konfig,0,$cpos+17).$today.substr((string)$konfig,$cpos+27);
//echo('<b>NEW:</b> '.$today.'<hr />');
	$konfig=strtr($konfig, array('{'=>"", '}'=>""));
}

// File name
$cfn=substr((string)$ddatei, 1+strrpos($ddatei, '/'));
//echo('Filename $cfn: ['.$cfn.']<hr />');
$cpos=strpos($konfig,'// File name   : ');
if($cpos !== false)
	$konfig=substr((string)$konfig,0,$cpos+17).$cfn.substr((string)$konfig,$cpos+17+strlen((string)$cfn));

// Add form name for Performance proof (2019) or Invoice (2021)?
$formvalue='Form '.substr((string)$kdatei,-7,3);
//echo('$formvalue=['.$formvalue.']<hr />');
if((($_POST['ktyp'] == 2019) || ($_POST['ktyp'] == 2021)) && ((substr((string)$_POST['kdatei'], 0, 1) == '_') || ($_POST['mordner'] != '0')))
{
	$formpos=strpos($konfig, '#form');
	if(!$formpos)
	{
		$add  = '// Name for this selectable form in the PDF File Manager'.$nl;
		$add .= '// -----------------------------------------------------------------------------'.$nl;
		$add .= '#form          Form '.substr((string)$kdatei,-7,3).$nl.$nl;

		$wpos=stripos($konfig, '// width');
		if($wpos !== false)
			$konfig=substr((string)$konfig,0,$wpos).$add.substr((string)$konfig,$wpos);
		else
			$konfig=$add.$konfig;
	}
	else
	{
		$formend=strpos($konfig,"\n", $formpos+5);
		$formvalue=trim(substr($konfig,$formpos+5, $formend-$formpos-5));
//echo('$formpos='.$formpos.', $formend='.$formend.' &rarr; $formvalue=['.$formvalue.']<hr />');
		if(!strlen($formvalue))
			$konfig=substr($konfig,0,$formpos).'#form              Form '.substr((string)$kdatei,-7,3).substr($konfig,$formend);
	}
}

// Perfomrance proof (2019)
if($_POST['ktyp'] == 2019)
{
	// Additional settings?
	$add='';
	// Setting: Don't display project numbers in the title section?
	$epos=stripos($konfig, '#t_noprocnr');
	if($epos === false)
		$add .= '//#t_noprocnr                  // Don\'t display project numbers in the title section'.$nl;
	// Setting: Don't display project path in the title section?
	$epos=stripos($konfig, '#t_nopath');
	if($epos === false)
		$add .= '//#t_nopath                    // Don\'t display project path in the title section'.$nl;

	if(strlen((string)$add))
	{
		$vpos=strpos($konfig, '//#f_nodate');
		if($vpos === false)
			$vpos=strpos($konfig, '#f_nodate');
		if($vpos !== false)
			$konfig=substr((string)$konfig,0,$vpos).$add.substr((string)$konfig,$vpos);
		else
			$konfig .= $nl.$add;
	}
}
// Invoice (2021)
else if($_POST['ktyp'] == 2021)
{
	$fpa=array();
	if(strlen($ndatei))
		$fpa=load_pdfconfig(2, $ndatei);
//fxDebug($fpa,'$fpa: $ndatei='.$ndatei, 0);

	// Additional communication variables top (#kto) or bottom (#ktu)?
	$kto='';
	$ktu='';

	$sql="SELECT tabwert FROM wertetab WHERE (wertetabid=2113 AND mandanten_id=0)";
	// ...Current client variables
	if($_POST['mordner'] == $GLOBALS['fxpglobals']['client'])
		$sql .= " OR (mandanten_id=".$GLOBALS['fxpglobals']['client']." AND id_feld=94)";
	$sql .= " ".db_orderby('tabwert');
	$cva=db_values($sql, '*');
//fxDebug($cva,$sql, 0);
	if(fxIsArray($cva))
	{
		$rva=array();
		foreach($cva as $cv)
		{
			$val=str_replace(' ', '_', strtolower((string)trim((string)$cv['tabwert'])));
			if(!isset($rva[$val]))
			{
				$ktv='kto_'.$val;
				if(!isset($fpa[$ktv]))
					$kto .= '//';
				$kto .= '#'.$ktv.$GLOBALS['nl_b'];

				$ktv='ktu_'.$val;
				if(!isset($fpa[$ktv]))
					$ktu .= '//';
				$ktu .= '#'.$ktv.$GLOBALS['nl_b'];

				$rva[$val]=0;
			}
			$rva[$val] += 1;
		}
	}
//fxDebug($rva,'$rva', 0);

//echo('<b>$kto:</b><br />'.$kto.'<hr /><b>$ktu:</b><br />'.$ktu.'<hr />');
	if(strlen((string)$kto) || strlen((string)$ktu))
	{
		$cspos=0;
		$fopos=false;
		$lopos=false;
		while(true)
		{
			$opos=strpos($konfig,'//#kto',$cspos);
			if($opos === false)
				$opos=strpos($konfig,'#kto',$cspos);
			if($opos === false)
				break;
			else
			{
				$cspos=strpos($konfig,"\n",$opos);
				if($fopos === false)
					$fopos=$opos;
				$lopos=$cspos+1;
			}
		}
//echo('$fopos=['.$fopos.'], $lopos=['.$lopos.']<hr />');

		$cspos=0;
		$fupos=false;
		$lupos=false;
		while(true)
		{
			$upos=strpos($konfig,'//#ktu',$cspos);
			if($upos === false)
				$upos=strpos($konfig,'#ktu',$cspos);
			if($upos === false)
				break;
			else
			{
				$cspos=strpos($konfig,"\n",$upos);
				if($fupos === false)
					$fupos=$upos;
				$lupos=$cspos+1;
			}
		}
//echo('$fupos=['.$fupos.'], $lupos=['.$lupos.']<hr />');
	}

	// Adjust invoice according to templace
	$ia=explode($GLOBALS['nl_b'], $intemp);
//fxDebug($ia,'$ia', 0);
	$nk='';
	$sl=false;
	foreach($ia as $il)
	{
		$il=trim($il);
//echo('<i>'.$il.'</i><br />');
		$li='';
		$iv=$il;
		if(substr($iv,0,2) == '//')
		{
			$li='//';
			$iv=trim(substr($iv,2));
		}

		if(substr($il,0,16) == '// File name   :')
			$il=substr($il,0,17).$cfn.str_repeat(' ',61-strlen($cfn)).'//';
		else if(substr($il,0,16) == '// Version     :')
			$il=substr($il,0,17).FXP_VERSION.str_repeat(' ',61-strlen(FXP_VERSION)).'//';
		else if(substr($il,0,16) == '// Last Change :')
			$il=substr($il,0,17).substr($GLOBALS['date'],0,4).'-'.substr($GLOBALS['date'],4,2).'-'.substr($GLOBALS['date'],6,2).str_repeat(' ',51).'//';
		else if(substr($iv,0,1) == '#')
		{
			$vn=$iv;
			$ns=strpos($vn,' ');
			$nt=strpos($vn,"\t");
			$nn=0;
			if(($ns !== false) && ($nt !== false))
				$nn=min($ns,$nt);
			else if($ns !== false)
				$nn=$ns;
			else if($nt !== false)
				$nn=$nt;
			if($nn)
				$vn=trim(substr($vn,0,$nn));
			$pv=strtolower(substr($vn,1));
			if(substr($pv,0,1) == '_')
				$pv=substr($pv,1);
			$vp=strpos($il,'{');
			$cp=strpos($il,'//', strlen($li));
//echo('&rarr; <b class=green>$vn='.$vn.', $pv='.$pv.', $li='.$li.' ($vp='.$vp.', $cp='.$cp.')</b><br />');
			// Replacements
			$rl='';
			// ...form
			if($vn == '#form')
				$rl=$vn.str_repeat(' ', $vp-strlen($vn)).$formvalue;
			// ...kto
			else if($vn == '#kto')
			{
				if(strlen($kto))
					$rl=trim($kto);
				else
					$rl=strtr($il, array('{'=>"", '}'=>""));
			}
			// ...ktu
			else if($vn == '#ktu')
			{
				if(strlen($ktu))
					$rl=trim($ktu);
				else
					$rl=strtr($il, array('{'=>"", '}'=>""));
			}
			// ...head/foot
			else if(($vn == '#head') || ($vn == '#foot'))
			{
				$rl=$vn;
				if(isset($fpa[$pv]))
					$rl .= str_replace('^', $GLOBALS['nl_b'].'^', $fpa[$pv]);
				$sl=true;
			}
			// ...fcx_note3 ++
			else if($vn == '#fcx_note3')
			{
				$rl='';
				$rn=3;
				$re=true;
				while($re)
				{
					if(strlen($rl))
						$rl .= $GLOBALS['nl_b'];
					$al='#'.$pv.str_repeat(' ', $vp-fxStrLen($vn));
					if(isset($fpa[$pv]))
						$al .= $fpa[$pv];
					if($cp > fxStrLen($al))
						$al .= str_repeat(' ', $cp-fxStrLen($al));
					else
						$al .= ' ';
					$al .= substr($il,$cp,-1).$rn;

					$rl .= $al;

					$rn++;
					$pv='fcx_note'.$rn;
					if(!isset($fpa[$pv]))
						$re=false;
				}
				$cp=0;
			}
			// ...[value]
			else if(isset($fpa[$pv]))
			{
				$rl=$vn;
				if($vp > fxStrLen($rl))
					$rl .= str_repeat(' ', $vp-fxStrLen($rl));
				else
					$rl .= ' ';
				if((is_string($fpa[$pv]) && strlen($fpa[$pv])) || (is_double($fpa[$pv]) && ($fpa[$pv] > 0.0)))
					$rl .= $fpa[$pv];
			}
			// ...[default]
			else if(!$sl)
			{
				$rl=strtr($il, array('{'=>"", '}'=>"  "));
				$cp=0;
			}
			// Comment
			if($cp > 0)
			{
				if($cp > fxStrLen($rl))
					$rl .= str_repeat(' ', $cp-fxStrLen($rl));
				else
					$rl .= ' ';
				$rl .= substr($il,$cp);
			}
			$il=$rl;
		}
		if(!$sl || (substr($il,0,1) != '^'))
			$nk .= $il.$GLOBALS['nl_b'];
	}
	$konfig=$nk;
}
$Text_varLang_style='font-family:Courier,sans-serif;font-style:normal;font-weight:normal;color:#006b9f;';
$maskenfeldwerte['text_varlang']=$konfig;
?>