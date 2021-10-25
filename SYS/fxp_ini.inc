<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : fxp_ini.inc                                                  //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
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
 * Initialization routine, i.e. language, database connection and environment settings
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

// Include initialization function
$GLOBALS['__includer']['init']=true;
require('includer.inc');

// Load default transaction session
fxSession('t0','l');
//fxDebug($_POST,'$_POST', 0);

// Database type
$GLOBALS['_dbtype']=0;
if(fxIsArray($_POST) && isset($_POST['fini_dbtype']))
	$GLOBALS['_dbtype']=(int)$_POST['fini_dbtype'];

// Get all langauges
$lg=0;
if(fxIsArray($_GET) && isset($_GET['uid']) && ($_GET['uid'] == 'fini_button_create'))
	$lg=-1;
$la=fxGetAllLanguages(true);
if(sizeof($la) && fxIsArray($_GET) && isset($_GET['uid']))
{
	foreach($la as $lid => $laa)
	{
		if($_GET['uid'] == 'IniLang'.$lid)
		{
			$lg=$lid;
			break;
		}
	}
}
//fxDebug($la,'$la: $lg='.$lg, 0);

// Select language
if(!$lg || (fxIsArray($_POST) && isset($_POST['fini_change_lang'])))
{
	$lg=0;
	$GLOBALS['fxpglobals']['lang']=0;
	$GLOBALS['set_fxheader']='';

	$content  = '<table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
	$content .= '	<tr>'.$GLOBALS['nl'];
	$content .= '		<td width="100%" height="100%" align="center" valign="middle">'.$GLOBALS['nl'];
	if(sizeof($la))
	{
		$content .= '			<table border="0" cellpadding="0" cellspacing="12">'.$GLOBALS['nl'];
		foreach($la as $lid => $lcta)
		{
			$content .= '				<tr>'.$GLOBALS['nl'];
			$content .= '					<td nowrap><img class="fxfbb" src="'.$lcta['fl'].'" align="top" style="height:28px;margin-right:6px;">'.fieldSubmit('IniLang'.$lid, $lcta['tx'], 'bsub', 180, false, false, false, '', '', '', 'font-size:14pt;font-weight:bold;background:#aaa;', true).'</td>'.$GLOBALS['nl'];
			$content .= '				</tr>'.$GLOBALS['nl'];
			if(strlen($GLOBALS['set_fxheader']))
				$GLOBALS['set_fxheader'] .= '/';
			$GLOBALS['set_fxheader'] .= $lcta['lt'];
		}
		$content .= '			</table>'.$GLOBALS['nl'];
	}
	else
	{
		$content .= '			<h1 class="red tshadow">There were no language definition files found in the subdirectory &quot;'.$GLOBALS['lngpath'].'&quot;!</b>'.$GLOBALS['nl'];
		$GLOBALS['set_fxheader']='Language';
	}
	$content .= '		</td>'.$GLOBALS['nl'];
	$content .= '	</tr>'.$GLOBALS['nl'];
	$content .= '</table>';

	echo($content);
}

// Change language
if($lg > 0)
	$GLOBALS['fxpglobals']['lang']=$lg;

if($lg)
{
	$GLOBALS['mskstyle']='float:left;top:0;width:49.2%;min-width:800px;min-height:128px;';

	// Load initialization texts in selected language
	if(!fxIsArray($GLOBALS['itla']))
		$GLOBALS['itla']=fxGetLanguageTexts('init', true);
//fxDebug($GLOBALS['itla'],'$GLOBALS[\'itla\']: fxGetLanguageTexts(\'init\')',0);

	// Check PHP extensions
	$pext=fxf_checkExtensions($GLOBALS['_dbtype']);
//fxDebug($pext,'$pext', 0);

	// Default variables
	$fierror=false;
	$slog='';
	$GLOBALS['dberrorlog']='';
	$GLOBALS['dbsucceslog']='';

	$content='';
	$settings=true;

	if(fxIsArray($_POST) && isset($_POST['fini_button_create']))
	{
		// Decode special values
		$GLOBALS['_username']=fxDecodeValue($_POST['fini_username']);
		$GLOBALS['_password']=fxDecodeValue($_POST['fini_password']);
		$GLOBALS['_smtp_email']=fxDecodeValue($_POST['fini_smtp_email']);
		$GLOBALS['_smtp_account']=fxDecodeValue($_POST['fini_smtp_account']);
		$GLOBALS['_smtp_password']=fxDecodeValue($_POST['fini_smtp_password']);
		$GLOBALS['_curl_proxy_username']=fxDecodeValue($_POST['fini_curl_proxy_username']);
		$GLOBALS['_curl_proxy_password']=fxDecodeValue($_POST['fini_curl_proxy_password']);

		// Integer values
		$GLOBALS['_dbprotocol']=(int)$_POST['fini_dbprotocol'];
		$GLOBALS['_smtp_port']=(int)$_POST['fini_smtp_port'];
		$GLOBALS['_curl_proxy_port']=(int)$_POST['fini_curl_proxy_port'];
		$GLOBALS['_sysadmin']=(int)$_POST['fini_sysadmin'];
		$GLOBALS['_elinks']=(int)$_POST['fini_elinks'];
		$GLOBALS['_scheduler']=(int)$_POST['fini_scheduler'];

		// String values
		$GLOBALS['_dbname']=trim($_POST['fini_dbname']);
		$GLOBALS['_dbserver']=trim($_POST['fini_dbserver']);
		$GLOBALS['_smtp_host']=trim($_POST['fini_smtp_host']);
		$GLOBALS['_smtp_helo']=trim($_POST['fini_smtp_helo']);
		$GLOBALS['_curl_proxy']=trim($_POST['fini_curl_proxy']);
		$GLOBALS['_email']=trim($_POST['fini_email']);

		// Check PHP extensions
		if(fxIsArray($pext) && strlen($pext['missing']))
		{
			$fierror=true;
			$slog .= '<b class="red">'.$GLOBALS['itla']['ex_error'].':</b><br /> <span class="lightgrey">&rarr;</span> '.$pext['missing'].'<br />';
		}

		// Check empty fields, e.g. database name, email
		if(!$fierror)
		{
			if(!strlen($GLOBALS['_dbname']))
			{
				$fierror=true;
				addFieldMessage('fini_dbname', $GLOBALS['itla']['db_name_error']);	// The database name cannot be empty
			}
			if(!strlen($GLOBALS['_email']))
			{
				$fierror=true;
				addFieldMessage('fini_email', $GLOBALS['itla']['em_error']);	// The email value cannot be empty
			}
		}

		// Create database if it does not exist already
		if(!$fierror)
		{
			// Database type
			$slog .= '<b class=underline>'.$GLOBALS['_dbtypes'][$GLOBALS['_dbtype']]['text'].'</b><br /><br />';

			// Check 1: Connecting to Server <!--#DBSERVER#--> as User <!--#USERNAME#--> with the entered password
			$slog .= strtr($GLOBALS['itla']['db_connect'], array('<!--#DBSERVER#-->'=>'[<b>'.$GLOBALS['_dbserver'].'</b>]', '<!--#USERNAME#-->'=>'[<b>'.$GLOBALS['_username'].'</b>]')).'<br />&nbsp;&rarr; ';
			$dbcon=false;
			// ...PostgreSQL
			if($GLOBALS['_dbtype'] == 1)
			{
				if(strlen($GLOBALS['_dbserver']))
				{
					$host_name=$GLOBALS['_dbserver'];
					$host='host='.$host_name.' ';
				}
				else
				{
					$host_name='localhost';
					$host='';
				}
				$dbcon=@pg_connect($host.'dbname=template1 user='.$GLOBALS['_username'].' password='.$GLOBALS['_password']);
			}
			// ...SQLServer
			else if($GLOBALS['_dbtype'] == 2)
			{
				$cia=array('UID'=>$GLOBALS['_username'], 'PWD'=>$GLOBALS['_password']);
				$dbcon=@sqlsrv_connect($GLOBALS['_dbserver'], $cia);
			}
			if($dbcon !== false)
			{
				// Success
				$slog .= '<b class="green">'.$GLOBALS['itla']['ms_success'].'</b><br /><br />';

				// Check 2: Checking if database/tablespace <!--#DBNAME#--> already exists
				$slog .= strtr($GLOBALS['itla']['db_exists'], array('<!--#DBNAME#-->'=>'[<b>'.$GLOBALS['_dbname'].'</b>]')).'<br />&nbsp;&rarr; ';
				$result=false;
				// ...PostgreSQL
				if($GLOBALS['_dbtype'] == 1)
				{
					$dbcon2=@pg_connect($host.'dbname='.$GLOBALS['_dbname'].' user='.$GLOBALS['_username'].' password='.$GLOBALS['_password']);
					$result=$dbcon2;
					if($result)
					{
						@pg_close($dbcon);
						$dbcon=$dbcon2;
					}
				}
				// ...SQLServer
				else if($GLOBALS['_dbtype'] == 2)
					$result=@sqlsrv_query($dbcon, "USE ".$GLOBALS['_dbname']);
				if(!$result)
				{
					// No
					$slog .= '<b class="green">'.$GLOBALS['itla']['ms_no'].'</b><br /><br />';

					// Action: Creating new database/tablespace <!--#DBNAME#-->
					$slog .= strtr($GLOBALS['itla']['db_create'], array('<!--#DBNAME#-->'=>'[<b>'.$GLOBALS['_dbname'].'</b>]')).'<br />&nbsp;&rarr; ';
					$result=false;
					// ...PostgreSQL
					if($GLOBALS['_dbtype'] == 1)
						$result=pg_query($dbcon, "CREATE DATABASE ".$_POST['fini_dbname']." ENCODING '".FXP_CHARSET."'");
					// ...SQLServer
					else if($GLOBALS['_dbtype'] == 2)
						$result=sqlsrv_query($dbcon, "CREATE DATABASE ".$GLOBALS['_dbname']);
					if($result)
					{
						// Success
						$slog .= '<b class="green">'.$GLOBALS['itla']['ms_success'].'</b><br /><br />';
					}
					else
					{
						// Error
						$fierror=true;
						$slog .= '<b class="red">'.$GLOBALS['itla']['ms_error'].'</b><br /><br />';
					}
				}
				else
				{
					// Yes
					$slog .= '<b class="green">'.$GLOBALS['itla']['ms_yes'].'</b><br /><br />';

					// Check 3: Checking if database/tablespace <!--#DBNAME#--> is empty, i.e. has no user tables
					$slog .= strtr($GLOBALS['itla']['db_empty'], array('<!--#DBNAME#-->'=>'[<b>'.$GLOBALS['_dbname'].'</b>]')).'<br />&nbsp;&rarr; ';
					$result=false;
					// ...PostgreSQL
					if($GLOBALS['_dbtype'] == 1)
					{
						$result=pg_query($dbcon, "SELECT tablename FROM pg_tables WHERE schemaname='public'");
						if($result != false)
							$result=pg_fetch_row($result);
					}
					// ...SQLServer
					else if($GLOBALS['_dbtype'] == 2)
					{
						$result=sqlsrv_query($dbcon, "SELECT name FROM sysobjects WHERE (type='U') AND (category=0)");
						if($result)
							$result=sqlsrv_fetch_array($result, SQLSRV_FETCH_ASSOC);
					}
					if($result)
					{
						// No
						$slog .= '<b class="lightgrey">'.$GLOBALS['itla']['ms_no'].'</b><br /><br />';

						// Check 4: Checking if the following tables already exist: <!--#TABLES#--> 
						$slog .= strtr($GLOBALS['itla']['db_lookup'], array('<!--#TABLES#-->'=>'[<b>tabellen</b>], [<b>benutzer</b>] + [<b>wertetab</b>]')).'<br />&nbsp;&rarr; ';
						$result=false;
						$cta=array('tabellen', 'benutzer', 'wertetab');
						foreach($cta as $ctn)
						{
							// ...PostgreSQL
							if($GLOBALS['_dbtype'] == 1)
							{
								$result=pg_query($dbcon, "SELECT tablename FROM pg_tables WHERE schemaname='public' AND LOWER(tablename) LIKE '".$ctn."'");
								if($result != false)
									$result=pg_fetch_row($result);
							}
							// ...SQLServer
							else if($GLOBALS['_dbtype'] == 2)
							{
								$result=sqlsrv_query($dbcon, "SELECT name FROM sysobjects WHERE (type='U') AND (category=0) AND name LIKE '".$ctn."'");
								if($result)
									$result=sqlsrv_fetch_array($result, SQLSRV_FETCH_ASSOC);
							}
							if(!$result)
								break;
						}
						if(!$result)
						{
							// No
							$fierror=true;
							$slog .= '<b class="red">'.$GLOBALS['itla']['ms_no'].'</b> [<i class="red">'.$ctn.'</i>]<br /><br />';
						}
						else
						{
							// Yes
							$slog .= '<b class="green">'.$GLOBALS['itla']['ms_yes'].'</b><br /><br />';

							// Check 5: Checking if this database version is less or equal to the current program version <!--#FXVERSION#-->
							$pgvs=FXP_VERSION;
							while(substr($pgvs,-2) == '.0')
								$pgvs=trim(substr($pgvs,0,-2));
							$pgvl='';
							$pgva=explode('.', $pgvs);
							for($v=0; $v<3; $v++)
							{
								if(isset($pgva[$v]))
									$pgvl .= substr('000'.(int)$pgva[$v], -3);
								else
									$pgvl .= '000';
							}
							$slog .= strtr($GLOBALS['itla']['db_version'], array('<!--#FXVERSION#-->'=>'[<b>'.$pgvs.'</b>]')).'<br />&nbsp;&rarr; ';
							$result=false;
							// ...PostgreSQL
							if($GLOBALS['_dbtype'] == 1)
							{
								$result=pg_query($dbcon, "SELECT tabwert, zeitstempel FROM wertetab WHERE wertetabid=10 AND id_sprache=1 AND mandanten_id=0");
								if($result != false)
									$result=pg_fetch_array($result, 0, PGSQL_ASSOC);
							}
							// ...SQLServer
							else if($GLOBALS['_dbtype'] == 2)
							{
								$result=sqlsrv_query($dbcon, "SELECT tabwert, zeitstempel FROM wertetab WHERE wertetabid=10 AND id_sprache=1 AND mandanten_id=0");
								if($result)
									$result=sqlsrv_fetch_array($result, SQLSRV_FETCH_ASSOC);
							}
							$dbvs='0';
							$dbvl='';
							if(fxIsArray($result) && isset($result['tabwert']))
							{
								$dbvs=$result['tabwert'];
								while(substr($dbvs,-2) == '.0')
									$dbvs=trim(substr($dbvs,0,-2));
								$dbva=explode('.', $dbvs);
								for($v=0; $v<3; $v++)
								{
									if(isset($dbva[$v]))
										$dbvl .= substr('000'.(int)$dbva[$v], -3);
									else
										$dbvl .= '000';
								}
							}
//$slog .= 'pgvl=[<b>'.$pgvl.'</b>], pgvs=[<b>'.$pgvs.'</b>] -- dbvl=[<b>'.$dbvl.'</b>], dbvs=[<b>'.$dbvs.'</b>] &rarr;';
							if(!strlen($dbvl) || ($dbvl > $pgvl))
							{
								// No
								$fierror=true;
								$slog .= '<b class="red">'.$GLOBALS['itla']['ms_no'].'</b> [<i class="red">'.$dbvs.'</i>]<br /><br />';
							}
							else
							{
								// Yes
								$slog .= '<b class="green">'.$GLOBALS['itla']['ms_yes'].'</b> [<i class="green">'.$dbvs.'</i>]<br /><br />';
							}
						}
					}
					else
					{
						// Yes
						$slog .= '<b class="green">'.$GLOBALS['itla']['ms_yes'].'</b><br /><br />';
					}
				}

				// Close connection
				if($dbcon)
				{
					// ...PostgreSQL
					if($GLOBALS['_dbtype'] == 1)
						@pg_close($dbcon);
					// ...SQLServer
					else if($GLOBALS['_dbtype'] == 2)
						@sqlsrv_close($dbcon);
				}
			}
			else
			{
				// Error
				$fierror=true;
				$slog .= '<b class="red">'.$GLOBALS['itla']['ms_error'].'</b><br /><br />';
			}
		}

		// Create the configuration file
		if(!$fierror)
		{
			$fiw=80;	// 80 characters wide
			$fsp=str_repeat(' ', $fiw);
			$fct='// ';
			$fc1=str_repeat('/', $fiw).$GLOBALS['nl_b'];
			$fc2=$fct.str_repeat('-', $fiw-strlen($fct)).$GLOBALS['nl_b'];

			// ...Default Language
			$fic=$fc1.$fct.strtoupper($GLOBALS['itla']['ln_default']).$GLOBALS['nl_b'].$fc2;
			foreach($la as $lid => $lia)
				$fic .= $fct.$lid.', '.$lia['cc'].' = '.$lia['tx'].$GLOBALS['nl_b'];
			$fic .= $fc1;
			$fic .= 'language = '.$GLOBALS['fxpglobals']['lang'].$GLOBALS['nl_b'];

			// ...OpenSSL Keys 32+64
			$GLOBALS['_oskey32']=base64_encode(openssl_random_pseudo_bytes(32));
			$GLOBALS['_oskey64']=base64_encode(openssl_random_pseudo_bytes(64));
			$fic .= fxf_iniAdd('ok', array('oskey32'=>'s', 'oskey64'=>'s'));

			// ...Database Connection Settings
			$fic .= fxf_iniAdd('db', array('dbtype'=>'s', 'dbserver'=>'t', 'dbname'=>'t', 'username'=>'p', 'password'=>'p', 'dbprotocol'=>'i'));

			// ...SMTP Settings
			$fic .= fxf_iniAdd('sm', array('smtp_host'=>'t', 'smtp_port'=>'i', 'smtp_helo'=>'t', 'smtp_email'=>'p', 'smtp_account'=>'p', 'smtp_password'=>'p'));

			// ...cURL Proxy Settings
			$fic .= fxf_iniAdd('cp', array('curl_proxy'=>'t', 'curl_proxy_port'=>'i', 'curl_proxy_username'=>'p', 'curl_proxy_password'=>'p'));

			// ...System Settings
			$fic .= fxf_iniAdd('sy', array('sysadmin'=>'s', 'elinks'=>'s', 'email'=>'t'));

			// ...Additional Settings
			$GLOBALS['_schedulername']='scheduler_'.hexPassword(16);
			$GLOBALS['_adminpassword']=hexPassword(16);
			$fic .= fxf_iniAdd('ad', array('scheduler'=>'s', 'schedulername'=>'s', 'adminpassword'=>'s'));
//echo('<pre>'.fxHtmlEncode($fic).'</pre>');

			// Save configuration file
			$fi_filename=fxf_fxIniFile(1);
			$fierror=fxSave($fi_filename, $fic, true);
			if(!$fierror)
				@chmod($fi_filename, 0755);
		}

		if(!$fierror)
			$settings=false;
//$settings=true;
	}

	// Display settings?
	if($settings)
	{
		// PHP Extensions
		$content .= '<div class="maskc" style="'.$GLOBALS['mskstyle'].'"><div class="maskh maskht" style="margin-bottom:4px;">&nbsp;'.$GLOBALS['itla']['ex_header'].':</div>'.$GLOBALS['nl'];
		$content .= '<table cellpadding="4">';
		if(fxIsArray($pext) && strlen($pext['ok']))
			$content .= '<tr><td nowrap><span class="lightgrey">&rarr;</span> <b class="green">'.$GLOBALS['itla']['ex_ok'].'</b></td><td nowrap><span class="grey">'.$pext['ok'].'</span></td></tr>'.$GLOBALS['nl'];
		if(fxIsArray($pext) && strlen($pext['notfound']))
			$content .= '<tr><td nowrap><span class="lightgrey">&rarr;</span> <b class="grey">'.$GLOBALS['itla']['ex_notloaded'].'</b></td><td nowrap><span class="grey">'.$pext['notfound'].'</span></td></tr>'.$GLOBALS['nl'];
		if(fxIsArray($pext) && strlen($pext['missing']))
		{
			$emsg='<b class="red">'.$GLOBALS['itla']['ex_error'].':</b><br /> <span class="lightgrey">&rarr;</span> '.$pext['missing'].'<br />';
			$content .= '<tr><td nowrap><span class="lightgrey">&rarr;</span> <b class="red">'.$GLOBALS['itla']['ex_missing'].'</b></td><td nowrap><span class="grey">'.$pext['missing'].'</span></td></tr>'.$GLOBALS['nl'];
			$GLOBALS['dberrorlog']=$emsg.'<br />'.$GLOBALS['dberrorlog'];
		}
		$content .= '</table></div>'.$GLOBALS['nl'];

		// ChartDirector Extension
		if(fxIsArray($pext) && (strlen($pext['cdok']) || strlen($pext['cdnotfound'])))
		{
			$content .= '<div class="maskc" style="'.$GLOBALS['mskstyle'].'"><div class="maskh maskht" style="margin-bottom:4px;">&nbsp;'.$GLOBALS['itla']['cd_header'].' ('.$GLOBALS['itla']['ms_version'].$pext['cdversf'].'):</div>'.$GLOBALS['nl'];
			$content .= '<table cellpadding="4">';
			if(strlen($pext['cdok']))
				$content .= '<tr><td nowrap><span class="lightgrey">&rarr;</span> <b class="green">'.$GLOBALS['itla']['cd_ok'].'</b></td><td nowrap><span class="grey">'.$pext['cdok'].'</span></td></tr>'.$GLOBALS['nl'];
			else
				$content .= '<tr><td nowrap><span class="lightgrey">&rarr;</span> <b class="grey">'.$GLOBALS['itla']['cd_notfound'].'</b></td><td nowrap><span class="grey">'.$pext['cdnotfound'].'</span></td></tr>'.$GLOBALS['nl'];
			$content .= '</table></div>'.$GLOBALS['nl'];
		}

		// Database Connection Settings
//fxDebug($GLOBALS['fxpglobals']['dbparam'],'dbparam', 0);
		$pp=strpos($GLOBALS['itla']['db_dbtype'],'|');
		if($pp)
			$GLOBALS['itla']['db_dbtype']=trim(substr($GLOBALS['itla']['db_dbtype'],0,$pp));
		$GLOBALS['itla']['db_dbtype'] .= '||';
		$_dbc=0;
		foreach($GLOBALS['_dbtypes'] as $_dbt => $_dba)
		{
			if($_dbc)
				$GLOBALS['itla']['db_dbtype'] .= ',';
			$GLOBALS['itla']['db_dbtype'] .= $_dbt.' = '.$_dba['text'];
			$_dbc++;
		}
		if(fxIsArray($pext) && fxIsArray($pext['extstat']) && $pext['extstat']['sqlsrv'])
		{
			$dbtd=2;
			$dbsd=$_SERVER['HTTP_HOST'];
			$dbnd='fxp';
			$dbud='sa';
		}
		else
		{
			$dbtd=1;
			$dbsd='';
			$dbnd='fxp';
			$dbud='postgres';
		}
		$content .= fxf_iniTableContent('db', array('dbprotocol'=>1, 'dbtype'=>$dbtd, 'dbserver'=>$dbsd, 'dbname'=>$dbnd, 'username'=>"#".$dbud, 'password'=>"*"));

		// SMTP Settings
		$content .= fxf_iniTableContent('sm', array('smtp_host'=>"", 'smtp_port'=>25, 'smtp_helo'=>"", 'smtp_email'=>"#", 'smtp_account'=>"#", 'smtp_password'=>"*"));

		// cURL Proxy Settings
		$content .= fxf_iniTableContent('cp', array('curl_proxy'=>"", 'curl_proxy_port'=>"", 'curl_proxy_username'=>"#", 'curl_proxy_password'=>"*"));

		// System Settings
		$content .= fxf_iniTableContent('sy', array('sysadmin'=>0, 'elinks'=>0, 'email'=>0));

		// Additional Settings
		$content .= fxf_iniTableContent('ad', array('scheduler'=>0));
	}

	if($settings)
	{
		echo($content);

		// Header + Buttons
		$GLOBALS['set_fxheader']=$GLOBALS['itla']['hl_ini'];	// Configuration File
		$button=fieldSubmit('fini_button_create', $GLOBALS['itla']['bt_create'], 'sub', 0, false, false, true);	// Create Configuration File
		$button=fieldSubmit('fini_change_lang', $GLOBALS['itla']['bt_lang'], 'sub', 0, false, false, true);		// Back to Language Selection
	}

	if($fierror)
		$GLOBALS['dberrorlog']=$slog;
	else
		$GLOBALS['dbsuccesslog']=$slog;

	if(strlen($GLOBALS['dberrorlog']))
	{
		$err=true;
		if($fierror)
			$GLOBALS['ema']=array('100','<span class=orange>'.$GLOBALS['itla']['cf_note'].'</span>',$GLOBALS['dberrorlog']);
		else
			$GLOBALS['ema']=array('100','<span class=red>'.$GLOBALS['itla']['cf_error'].'</span>',$GLOBALS['dberrorlog']);
	}
	else if(strlen($GLOBALS['dbsuccesslog']))
		$GLOBALS['ema']=array('001','<span class=green>'.$GLOBALS['itla']['cf_success'].'</span>',$GLOBALS['dbsuccesslog']);
}
?>