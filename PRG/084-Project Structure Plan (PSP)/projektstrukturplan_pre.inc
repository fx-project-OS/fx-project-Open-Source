<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : projektstrukturplan_pre.inc                                  //
// Version     : 21.2                                                         //
// Begin       : 2021-01-11                                                   //
// Last Change : 2021-01-11                                                   //
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
 * PF 84: Project Structure Plan (PSP)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

if($Button_Submit && fxIsArray($sel) && $top_pid)
{
	// PSP in Funktion ermitteln...
	$sel['v_buffer']=$buffer;
	$sel['v_cpath']=$cpath;
	$sel['v_top_pid']=$top_pid;
	$prj_info_arr=psp($top_pid, $sel, $dmode);
//fxDebug($prj_info_arr, '$prj_info_arr');
//fxDebug($prj_info_arr['var'],'var');

	// ...und anzeigen
	if(fxIsArray($prj_info_arr))
	{
		$sel['prj_info_arr']=$prj_info_arr;
		$GLOBALS['fxptdata']['pdf']=$sel;

		$trr189=tr_rechte(189);
		$trr23=tr_rechte(23);

		$type_array=kategorie_array('projektart');			// Projektartbezeichnung
		$prio_array=fxf_getReferenceValues('prioritaet');			// Prioritätsbezeichnungen
		$stat_array=fxf_getReferenceValues('projekt_status');		// Projektstatusbezeichnungen

		$txt_open	= '<i>{'.meldung(1794, false).'}</i>';	// offen
		$txt_nstart	= '<i>{'.meldung(2833, false).'}</i>';	// nocht nicht begonnen
		$txt_lit	= explode('|', meldung(2831, false));	// Vorgangsart|Status|Priorität|Planungszeitraum (Soll)|Arbeitszeitraum (Ist)|Aktionsnachricht|Int. Budgets (verbr./gen.)|Aufwand (vebr./gepl.)

		$top_vn		= '';

		$ewidth		= 132-2;
		$ewidth2	= floor($ewidth/2);
		$eheight	= 76-2;
		$eheight2	= floor($eheight/2);
		$egap		= 16;
		$egap2		= floor($egap/2);
		$edist		= 24;

		$bpadding	= 16;
		$bwidth		= 2*$bpadding + $prj_info_arr['var']['columns']*$ewidth  + ($prj_info_arr['var']['columns']-1)*$edist + $prj_info_arr['var']['gaps']*$egap;
		$bheight	= 2*$bpadding + ($prj_info_arr['var']['lines']+1)*$eheight  + $prj_info_arr['var']['lines']*$egap;
//echo('$bwidth='.$bwidth.', $bheight='.$bheight.'<br />');

		$div  = '<div id="psp" style="position:relative;width:'.$bwidth.'px;height:'.$bheight.'px;border:0;margin:0;padding:0;">'.$nl;
		if($dmode)
		{
			$ltop=$bpadding + $prj_info_arr['var']['ftl']*($eheight+$egap) - $egap/2;
			$div .= '<div style="position:absolute;left:'.($bpadding/2).'px;right:0;top:'.$ltop.'px;bottom:0;border:1px solid #ebebeb;border-radius:12px;margin:0;padding:0;"></div>'.$nl;
		}
		foreach($prj_info_arr as $vn => $data)
		{
			if($vn != 'var')
			{
				if(!strlen($top_vn))
					$top_vn=$vn;

				$etop=$bpadding + $data['line']*($eheight+$egap);

				$lcolumn=$data['lcolumn'];
				$rcolumn=$data['rcolumn'];
				$dcolumns=$rcolumn+1-$lcolumn;
//echo('<b class="blue">'.$vn.'</b>: $lcolumn='.$lcolumn.', $rcolumn='.$rcolumn.' - $dcolumns='.$dcolumns.'<br />');
				if((!$prj_info_arr['var']['dmode'] && !$data['line']) || ($prj_info_arr['var']['dmode'] && !$data['dtasks'] && ($dcolumns > 1)))
				{
					$cc=$lcolumn+floor($dcolumns/2);
					$gap=$data['gap'];
					for($c=0; $c<=$cc; $c++)
						$gap += $prj_info_arr['var']['gpc'][$c];
					$crl=$bpadding + $cc*($ewidth+$edist) + $gap*$egap;
//echo('- $cc='.$cc.', $gap='.$gap.' - $crl='.$crl);

					if(!($dcolumns%2))
					{
						$cc--;
						$cll=$bpadding + $cc*($ewidth+$edist) + $gap*$egap;
//echo(' -- $cc='.$cc.', $gap='.$gap.' - $cll='.$cll);

						$eleft=$cll + floor(($crl-$cll)/2);
					}
					else
						$eleft=$crl;
//echo(' -- $eleft=<b>'.$eleft.'</b><hr />');
				}
				else
				{
					$gap=$data['gap'];
					for($c=0; $c<=$lcolumn; $c++)
						$gap += $prj_info_arr['var']['gpc'][$c];
					$eleft=$bpadding + $lcolumn*($ewidth+$edist) + $gap*$egap;
//echo('- $gap='.$gap.' - $eleft=<b>'.$eleft.'</b><hr />');
				}

				$prj_info_arr[$vn]['top']=$etop;
				$prj_info_arr[$vn]['left']=$eleft;

				// Shadow + Border width
				$shadow='';
				if(!$data['type'])
					$shadow=' box-shadow:3px 3px 6px #000000;';

				// Border
				$borderstyle='1px '.$data['disp']['border-style'].' #'.$data['disp']['border-color'];

				// Background
				if(strlen($data['disp']['image']))
					$background='URL('.$GLOBALS['gfxpath'].'psp_'.$data['disp']['image'].$data['disp']['image-add'].'.png)';
				else
					$background='#ffffff';

				// Tooltip
				$lcol='f0f0f0';
				$tcol='f8f8f8';
				$ptt =' tooltip="&nbsp;<b class=black>'.$data['nr'].'</b>&nbsp;<font class=black>'.strtr($data['name'], $GLOBALS['caption_strtr']).'</font><br /><br />';
				$ptt .= '<table border=0 cellpadding=2 cellspacing=1>';
				// ...Vorgangsart
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[0].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.$type_array[$data['ptype']].'</td></tr>';
				// ...Status
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[1].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.$stat_array[$data['stat']].'</td></tr>';
				// ...Priorität
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[2].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.$prio_array[$data['prio']].'</td></tr>';
				// ...Planungszeitraum (Soll)
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[3].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.get_entry($data['ps'], "datum").' - '.get_entry($data['pe'], "datum").'</td></tr>';
				// ...Arbeitszeitraum (Ist)
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[4].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>';
				if(strlen($data['as']))
				{
					$ptt .= get_entry($data['as'], "datum").' - ';
					if(strlen($data['ae']))
						$ptt .= get_entry($data['ae'], "datum");
					else
						$ptt .= $txt_open;	// offen
				}
				else
					$ptt .= $txt_nstart;	// noch nicht begonnen
				$ptt .= '</td></tr>';
				// ...Int. Budgets (verbr./gen.)
				if(isset($data['bapp']) && isset($data['buse']))
					$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[6].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.get_entry($data['buse'], "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung'].' / '.get_entry($data['bapp'], "dezimal").' '.$GLOBALS['fxpglobals']['settings']['waehrung'].'</td></tr>';
				// ...Aufwand (vebr./gepl.)
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[7].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.get_entry($data['effa'], "dezimal").' h / '.get_entry($data['effp'], "dezimal").' h</td></tr>';

				$lcol='e0e0e0';
				$tcol='ffffff';
				if(strlen($data['disp']['image']))
				{
					if($data['disp']['image'] == 'ok')
						$tcol='e8ffe8';
					else if($data['disp']['image'] == 'ko')
						$tcol='ffe8e8';
					else if($data['disp']['image'] == 'wn')
						$tcol='ffffe8';
					else
						$tcol='eeeeee';
				}
				// ...Aktionsnachricht
				$ptt .= '<tr><td bgcolor=#'.$lcol.' nowrap valign=top><b>'.$txt_lit[5].'</b>:</td><td bgcolor=#'.$tcol.' nowrap>'.$data['tt'].'</td></tr>';
				$ptt .= '</table>"';

				// Beim Klick auf das Mutterprojekt nach Vorgänge (Matrix) springen
				$dlnk='';
				$cursor='';
				if(($data['pid'] == $data['mpid']) && $trr189)
					$dlnk=' '.fxf_jsFunction('loadTR', '>1', '^loading', 189, 'jump&keep_get=1&tr=189&aktion=3&pid='.(int)$data['pid']);
				// sonst nach Vorgänge (Detail) springen
				if(!strlen($dlnk) && $trr23)
					$dlnk=' '.fxf_jsFunction('loadTR', '>1', '^loading', 23, 'jump&keep_get=1&tr=23&aktion=3&pprojektid='.(int)$data['pid']);
				if(strlen($dlnk))
					$cursor=' cursor:pointer;';

				$div .= '	<div style="position:absolute; top:'.$etop.'px; left:'.$eleft.'px; width:'.$ewidth.'px; height:'.$eheight.'px; border:'.$borderstyle.'; border-radius:8px; background:'.$background.'; margin:0; padding:0;'.$shadow.$cursor.'"'.$ptt.$dlnk.'>'.$nl;

				$ename=$data['name'];
				if(strlen($ename) > 20)
				{
					if(strlen($ename) > 50)
						$ename=substr($ename, 0, 50).'...';
					$ename=text_wrap($ename, true, 20);
				}
				$div .= '		<div style="position:absolute;top:2px;left:1px;width:'.($ewidth-2).'px;height:'.($eheight-6).'px;text-align:center;overflow:hidden;white-space:nowrap;"><b>'.$data['nr'].'</b><br /><div class="s2 darkgrey" style="position:relative;top:8px;">'.$ename.'</div></div>'.$nl;

				$div .= '	</div>'.$nl;

				if(!strlen($prj_info_arr['var']['cp_vn']) || !$data['cp'])
					$div .= pspLine($vn, 'aaaaaa');
			}
		}

		// Kritischer Pfad
		if($cpath && strlen($prj_info_arr['var']['cp_vn']))
		{
			$vn=$prj_info_arr['var']['cp_vn'];
			while(strlen($vn))
			{
				$div .= pspLine($vn, 'e1001a');
				$vn=$prj_info_arr[$vn]['parent'];
			}
		}

		$div .=  '</div>'.$nl;

		// Literal: Mutterprojektbezeichnung
		$nv_text1_lit=$prj_info_arr[$top_vn]['nr'].':&nbsp;&nbsp;'.$prj_info_arr[$top_vn]['name'];
		// DIV-PSP in Maske einbinden
		$GLOBALS['fxptdata']['lvalues'][$GLOBALS['_maskcounter']][0]['nv_text1']='~'.$div;

		// Leerzeile mit Top Border
		fxf_maskColumnBorder(array('nv_text1'), 1, -1, $GLOBALS['_maskcounter'], 1, true);
	}
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Linie zeichnen
 *
 * @param various $vn     - Mandatory parameter:                      ???PARAMETER???
 * @param string  $lcolor - Optional parameter (default = '000000'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function pspLine($vn, $lcolor='000000')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	global $nl;

	$data=$GLOBALS['prj_info_arr'][$vn];
	$div ='';
	if($data['line'] > 0)
	{
		$pvn=$data['parent'];

		// ...Vom Teilprojekt zum übergeordneten (Teil)Projekt
		if((!$GLOBALS['prj_info_arr']['var']['dmode'] && ($data['line'] == 1)) || ($GLOBALS['prj_info_arr']['var']['dmode'] && !$data['type']))
		{
			$x1=$GLOBALS['prj_info_arr'][$pvn]['left'] + $GLOBALS['ewidth2'] + 2;
			$y1=$GLOBALS['prj_info_arr'][$pvn]['top'] + $GLOBALS['eheight'] + 2;
			$y2=$GLOBALS['prj_info_arr'][$vn]['top'];

			if($GLOBALS['prj_info_arr'][$pvn]['left'] == $GLOBALS['prj_info_arr'][$vn]['left'])
			{
				$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.$x1.'px; width:1px; height:'.($y2-$y1).'px; border:0; background-color:#'.$lcolor.'; overflow:hidden; white-space:nowrap;"></div>'.$nl;
			}
			else
			{
				$x2=$GLOBALS['prj_info_arr'][$vn]['left'] + $GLOBALS['ewidth2'] + 2;
				$ym=$y1 + floor(($y2-$y1)/2);
				$xm=min($x1,$x2);

				$div .= '	<div style="position:absolute; top:'.$ym.'px; left:'.$x2.'px; width:1px; height:'.($y2-$ym).'px; border:0; background-color:#'.$lcolor.'; overflow:hidden; white-space:nowrap;"></div>'.$nl;
				$div .= '	<div style="position:absolute; top:'.$ym.'px; left:'.$xm.'px; width:'.abs($x2-$x1).'px; height:1px; border:0; background-color:#'.$lcolor.'; overflow:hidden; white-space:nowrap;"></div>'.$nl;
				$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.$x1.'px; width:1px; height:'.($ym-$y1).'px; border:0; background-color:#'.$lcolor.'; overflow:hidden; white-space:nowrap;"></div>'.$nl;
			}
		}
		// ...zum Elter
		else
		{
			$x1=$GLOBALS['prj_info_arr'][$pvn]['left'] + $GLOBALS['egap2'];
			$y1=$GLOBALS['prj_info_arr'][$pvn]['top'] + $GLOBALS['eheight'] + 2;
			$x2=$GLOBALS['prj_info_arr'][$vn]['left'];
			$y2=$GLOBALS['prj_info_arr'][$vn]['top'] + $GLOBALS['eheight2'] + 2;

			$div .= '	<div style="position:absolute; top:'.$y1.'px; left:'.$x1.'px; width:1px; height:'.($y2-$y1).'px; border:0; background-color:#'.$lcolor.'; overflow:hidden; white-space:nowrap;"></div>'.$nl;
			$div .= '	<div style="position:absolute; top:'.$y2.'px; left:'.$x1.'px; width:'.($x2-$x1).'px; height:1px; border:0; background-color:#'.$lcolor.'; overflow:hidden; white-space:nowrap;"></div>'.$nl;
		}
	}

	return $div;
}
?>