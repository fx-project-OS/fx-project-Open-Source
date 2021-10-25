<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : init.inc                                                     //
// Version     : 21.2                                                         //
// Begin       : 2020-09-02                                                   //
// Last Change : 2021-09-27                                                   //
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
 * Collection of functions used during the fx-project initialization
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Draw the table mask for a specific initialization setting
 *
 * @param string $prefix - Mandatory parameter:  Prefix of the global texts from the LNG-subdirectory
 * @param array $fa      - Mandatory parameter:  Array containing all subsettings
 *
 * @return HTML string of the teable
 */
function fxf_iniTableContent($prefix, $fa)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$tar=array();
	if(fxIsArray($fa))
	{
		$prefix=trim($prefix);
		if(strlen($prefix))
			$prefix .= '_';

		if(!fxIsArray($GLOBALS['itla']))
			$GLOBALS['itla']=fxGetLanguageTexts('init', true);

		foreach($fa as $fn => $fv)
		{
			$v=explode('|', $GLOBALS['itla'][$prefix.$fn]);
//fxDebug($v,$fn, 0);

			$ft='';
			if((substr($fv,0,1) == '#') || (substr($fv,0,1) == '*'))
			{
				$ft=substr($fv,0,1);
				$fv=substr($fv,1);
			}

			$tar['fini_'.$fn]=array('text'=>$v[0], 'note'=>"", 'value'=>$fv, 'field'=>$ft);
			if(sizeof($v) > 1)
				$tar['fini_'.$fn]['note']=$v[1];
			if(sizeof($v) > 2)
				$tar['fini_'.$fn]['field'] .= $v[2];
		}
	}
//fxDebug($tar,'$tar', 0);

	$tcontent = '<div class="maskc" style="'.$GLOBALS['mskstyle'].'"><div class="maskh maskht" style="margin-bottom:4px;">&nbsp;'.$GLOBALS['itla'][$prefix.'header'].':</div>'.$GLOBALS['nl'];
	$tcontent .= '<table width="100%" cellpadding="4">';

	if(fxIsArray($tar))
	{
		$trc=0;
		foreach($tar as $tfn => $tfa)
		{
			$pfn=substr($tfn,5);

			if($trc)
				$tcontent .= '<tr><td colspan="2" style="border-bottom:1px dotted #ddd;"></tr>'.$GLOBALS['nl'];

			$tcontent .= '<tr>';
			// Literal
			$tcontent .= '<td nowrap valign="top" style="min-width:360px;"><span class="lightgrey">&rarr;</span> '.$tfa['text'].'<br />';
			if(strlen($tfa['note']))
				$tcontent .= '&nbsp;&nbsp;&nbsp;&nbsp;<i class="s1 lightgrey">'.$tfa['note'].'</i>';
			$tcontent .= '<br /></td>';
			// Value
			$val='';
			if(isset($GLOBALS[$tfn]))
				$val=$GLOBALS[$tfn];
			else if(fxIsArray($_POST) && isset($_POST[$tfn]))
				$val=$_POST[$tfn];
			else if(fxIsArray($GLOBALS['fxpglobals']['dbparam']) && isset($GLOBALS['fxpglobals']['dbparam'][$pfn]))
			{
				$ds3=substr($GLOBALS['fxpglobals']['dbparam'][$pfn],0,3);
				if(($ds3 != '~µ#') && ($ds3 != '~?#'))
					$val=$GLOBALS['fxpglobals']['dbparam'][$pfn];
				else if(function_exists('fxsppPC'))
					$val=fxsppPC($GLOBALS['fxpglobals']['dbparam'][$pfn],-1);
			}
			else
				$val=$tfa['value'];
//echo('<b>'.$tfn.'</b> <font class="grey">(</font>'.$pfn.'<font class="grey">)</font> = <font class="grey">[</font>'.$val.'<font class="grey">]</font><br />');
			// Field
			$ft='';
			if((substr($tfa['field'],0,1) == '#') || (substr($tfa['field'],0,1) == '*'))
			{
				$ft=substr($tfa['field'],0,1);
				$tfa['field']=substr($tfa['field'],1);
			}
			$tcontent .= '<td width="100%" nowrap valign="top">';
			if(strlen($tfa['field']))	// Select Field
			{
				$GLOBALS[$tfn.'select']=array();
				$sa=explode(',', $tfa['field']);
				foreach($sa as $sc => $st)
				{
					$ep=strpos($st,'=');
					if($ep)
					{
						$sc=(int)trim(substr($st,0,$ep));
						$st=trim(substr($st,$ep+1));
					}
					$GLOBALS[$tfn.'select'][$sc]=$st;
				}
				$GLOBALS[$tfn.'_ds']=0;
				$tcontent .= fieldSelect($tfn, $val, 400);
			}
			else if($ft == '*')			// Password Input Field
				$tcontent .= fieldPassword($tfn, $val, 400, 255);
			else if($ft == '#')			// Coded Input Field
				$tcontent .= fieldCode($tfn, $val, 400, 255);
			else						// Text Input Field
			{
				// Mandatory fields
				$mand=false;
				if(($tfn == 'fini_dbname') || ($tfn == 'fini_email'))
					$mand=true;
				$tcontent .= fieldText($tfn, $val, 400, 255, 'left', $mand);
			}
			$tcontent .= '</td>';
			$tcontent .= '</tr>'.$GLOBALS['nl'];

			$trc++;
		}
	}

	$tcontent .= '</table></div>'.$GLOBALS['nl'];

	return $tcontent;
}

/**
 * Draw the specific initialization setting
 *
 * @param string $prefix - Mandatory parameter:  Prefix of the global texts from the LNG-subdirectory
 * @param array $sa      - Mandatory parameter:  Array containing all subsettings
 *
 * @return HTML string of the teable
 */
function fxf_iniAdd($prefix, $sa)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fic='';
	if(fxIsArray($sa))
	{
		$umla=array('ä'=>'AE', 'ö'=>'OE', 'ü'=>'UE', 'ß'=>'SS', 'Ä'=>'AE', 'Ö'=>'OE', 'Ü'=>'UE');

		$prefix=trim($prefix);
		if(strlen($prefix))
			$prefix .= '_';

		if(!fxIsArray($GLOBALS['itla']))
			$GLOBALS['itla']=fxGetLanguageTexts('init', true);

		if(isset($GLOBALS['fiw']))
			$fiw=$GLOBALS['fiw'];
		else
			$fiw=80;	// 80 characters wide
		if(isset($GLOBALS['fsp']))
			$fsp=$GLOBALS['fsp'];
		else
			$fsp=str_repeat(' ', $fiw);
		if(isset($GLOBALS['fsc']))
			$fsc=$GLOBALS['fsc'];
		else
			$fct='// ';
		if(isset($GLOBALS['fc1']))
			$fc1=$GLOBALS['fc1'];
		else
			$fc1=str_repeat('/', $fiw).$GLOBALS['nl_b'];
		if(isset($GLOBALS['fc2']))
			$fc2=$GLOBALS['fc2'];
		else
			$fc2=$fct.str_repeat('-', $fiw-strlen($fct)).$GLOBALS['nl_b'];

		$sl=0;
		foreach($sa as $sf => $st)
			$sl=max($sl, strlen($sf));

		$fic .= $GLOBALS['nl_b'].$fc1.$fct.strtoupper(strtr($GLOBALS['itla'][$prefix.'header'], $umla)).$GLOBALS['nl_b'];
		// Note
		if(isset($GLOBALS['itla'][$prefix.'note']))
			$fic .= $fc2.text_wrap($GLOBALS['itla'][$prefix.'note'], true, $fiw-3, $fct, $GLOBALS['nl_b'], '');
		// Settings parameter
		foreach($sa as $sf => $st)
		{
			$fin='';
			if($sf == 'dbtype')
			{
				$_dbc=0;
				foreach($GLOBALS['_dbtypes'] as $_dbt => $_dba)
				{
					if($_dbc)
						$fin .= ',';
					$fin .= $_dbt.' = '.$_dba['text'];
					$_dbc++;
				}
			}
			else if($st != 'p')
			{
				$v=explode('|', $GLOBALS['itla'][$prefix.$sf]);
//fxDebug($v,$fn, 0);
				if(sizeof($v) > 2)
					$fin=$v[2];
				else if(($sf == 'email') && (sizeof($v) > 1))
					$fin=$v[1];
			}
			if(strlen($fin))
			{
				$fic .= $fc2;
				$fa=explode(',', $fin);
				foreach($fa as $c => $t)
				{
					$fic .= $fct;
					if(!$c)
						$fic .= substr($sf.':'.$fsp,0,$sl+4);
					else
						$fic .= substr($fsp,0,$sl+4);
					$fic .= trim($t).$GLOBALS['nl_b'];
				}
			}
		}
		$fic .= $fc1;
		foreach($sa as $sf => $st)
		{
			$sv=$GLOBALS['_'.$sf];
			if($st == 'i')
				$sv=(int)$sv;
			$fic .= substr($sf.$fsp,0,$sl).' = '.$sv.$GLOBALS['nl_b'];
		}
	}

	return $fic;
}
?>