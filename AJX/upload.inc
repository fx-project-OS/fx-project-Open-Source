<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : upload.inc                                                   //
// Version     : 21.1                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-09-21                                                   //
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
 * Program called from JavaScript to handle file, picture and document uploads
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */


// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

if(is_array($_GET) && isset($_GET['nohtml']))
	$html=0;
else
	$html=1;

if(is_array($_GET) && isset($_GET['frm']))
	$frm=trim($_GET['frm']);
else if(is_array($_POST) && isset($_POST['frm']))
	$frm=trim($_POST['frm']);
else
	$frm='';

if(is_array($_GET) && isset($_GET['utype']))
	$utype=strtolower(substr(trim($_GET['utype']),0,3));
else
	$utype='';

if(is_array($_GET) && isset($_GET['uparam']))
	$uparam=trim($_GET['uparam']);
else
	$uparam='';

if(is_array($_GET) && isset($_GET['umulti']))
	$umulti=(int)$_GET['umulti'];
else
	$umulti=0;

if(is_array($_GET) && isset($_GET['spath']))
	$spath=fxReplaceSpecialCharacters(trim($_GET['spath']), true, false, false, true);
else
	$spath='';

if(is_array($_GET) && isset($_GET['seid']))
	$seid=fxReplaceSpecialCharacters(trim($_GET['seid']), true, false, false, true);
else
	$seid='';

$uid='';
if(sizeof($_FILES))
	$uid=key($_FILES);

uploadFilesDebug('*');

if($html)
{
	$jfn=fxf_jsFunction('uploadFileDone', '!name');

	echo('<html>');

	echo('<head>');
	echo('<script type="text/javascript">');
	echo('	function uinit() {if(top.'.$jfn.') top.'.$jfn.'(\''.$uid.'\', \'\');}');
	echo('	window.onload=uinit;');
	echo('</script>');
	echo('</head>');

	echo('<body>');
}

$rtxt='';

if(sizeof($_FILES))
{
	if(is_array($_FILES[$uid]) && sizeof($_FILES[$uid]))
	{
		$v=array();
		if(is_array($_FILES[$uid]['name']) && sizeof($_FILES[$uid]['name']))
		{
			$faa=array();
			foreach($_FILES[$uid]['name'] as $fc => $fv)
			{
				$faa[]=array(
					'name'		=> $_FILES[$uid]['name'][$fc],
					'type'		=> $_FILES[$uid]['type'][$fc],
					'tmp_name'	=> $_FILES[$uid]['tmp_name'][$fc],
					'error'		=> $_FILES[$uid]['error'][$fc],
					'size'		=> $_FILES[$uid]['size'][$fc]
				);
			}
			$multi=true;
		}
		else
		{
			$faa[]=$_FILES[$uid];
			$multi=false;
		}

		$uf_ea=array('name'=>"", 'tmp'=>"", 'new'=>"", 'type'=>"", 'size'=>"", 'icon'=>"", 'width'=>"", 'height'=>"");
		$uf_ko=0;
		$uf_ok=0;
		$msg='';

		foreach($faa as $fc => $fa)
		{
			$uf_oname=$fa['name'];
			$uf_mname='';
			if(function_exists('mb_detect_encoding'))
			{
				$oe=mb_detect_encoding($uf_oname, 'UTF-8, ISO-8859-1, ISO-8859-15', true);
				if($oe != FXP_CHARSET)
					$uf_mname=mb_convert_encoding($uf_oname, FXP_CHARSET, $oe);
			}
			$uf_iname='';
			if(function_exists('iconv'))
				$uf_iname=trim(iconv('', FXP_CHARSET, $uf_oname));
			$uf_name=$uf_mname;
			if(strlen($uf_name) != strlen($uf_oname))
				$uf_name=$uf_iname;
			if(strlen($uf_name) != strlen($uf_oname))
				$uf_name=$uf_oname;
			$uf_type=$fa['type'];
			$uf_tmp=$fa['tmp_name'];
			$uf_error=$fa['error'];
			$uf_size=$fa['size'];
			$uf_icon='';
			$uf_width=0;
			$uf_height=0;

			// File icon from suffix
			$sdp=strrpos($uf_name, '.');
			if($sdp !== false)
			{
				$suffix=strtolower(substr($uf_name,$sdp+1));
				if(file_exists($GLOBALS['gfxpath'].'ico_'.$suffix.'.png'))
					$uf_icon=$suffix;
			}

			$v[$fc]=array('uid'=>$uid, 'error'=>$uf_error, 'oname'=>$uf_oname, 'mname'=>$uf_mname, 'iname'=>$uf_iname, 'name'=>$uf_name, 'tmp_name'=>$uf_tmp, 'new_name'=>"", 'type'=>$uf_type, 'size'=>$uf_size, 'icon'=>$uf_icon, 'width'=>$uf_width, 'height'=>$uf_height);

			if(strlen($msg))
				$msg .= '</div><br /><hr class="lightergrey" size="1" /><br />';
			$pl=12;
			if(sizeof($faa) > 1)
			{
				$pl=40;
				$msg .= '<span class="lightgrey">'.($fc+1).'/'.sizeof($faa).':</span>&nbsp;&nbsp;&nbsp;';
			}
			$msg .= '<b class=darkgrey>'.$uf_name.'</b><div style="float:right;"><i class="s2 lightgrey">('.convBytes($uf_size,'K',true).')</i></div>';
			$msg .= '<div style="padding-left:'.$pl.'px;padding-top:10px;">';

			if($uf_error == UPLOAD_ERR_EXTENSION)	// Cancel
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
					$msg .= '<b class="red">Der Upload wurde abgebrochen!</b>';
				else
					$msg .= '<b class="red">The upload has been canceled!</b>';
				$uf_ok=-1;
				$uf_ko=-1;
				break;
			}
			else if($uf_error && ($uf_error != UPLOAD_ERR_OK))
			{
				if($GLOBALS['fxpglobals']['lang'] == 1)
				{
		    		$ea=array(
						UPLOAD_ERR_INI_SIZE   => 'Die Groesse der Datei ueberschreitet den [upload_max_filesize] Wert in der php.ini!',
						UPLOAD_ERR_FORM_SIZE  => 'Die Groesse der Datei ueberschreitet den im HTML Form angegebenen [MAX_FILE_SIZE] Wert!',
						UPLOAD_ERR_PARTIAL    => 'Die Datei konnte nicht vollstaendig geuplaodet werden!',
						UPLOAD_ERR_NO_FILE    => 'Es wurde keine Datei geuploadet!',
						UPLOAD_ERR_NO_TMP_DIR => 'Es existiert kein temporaerer Upload-Ordner!',
						UPLOAD_ERR_CANT_WRITE => 'Die Datei konnte nicht gespeichert werden!'
					);
				}
				else
				{
		    		$ea=array(
						UPLOAD_ERR_INI_SIZE   => 'The uploaded file exceeds the [upload_max_filesize] directive in php.ini!',
						UPLOAD_ERR_FORM_SIZE  => 'The uploaded file exceeds the [MAX_FILE_SIZE] directive that was specified in the HTML form!',
						UPLOAD_ERR_PARTIAL    => 'The uploaded file was only partially uploaded!',
						UPLOAD_ERR_NO_FILE    => 'No file was uploaded!',
						UPLOAD_ERR_NO_TMP_DIR => 'Missing a temporary folder!',
						UPLOAD_ERR_CANT_WRITE => 'Failed to write file to disk!'
					);
				}
				$msg .= '<span class="red">';
				if(isset($ea[$uf_error]))
					$msg .= $ea[$uf_error];
				else if($GLOBALS['fxpglobals']['lang'] == 1)
					$msg .= 'Unbekannter Fehler!';
				else
					$msg .= 'Unknown error!';
				$msg=str_replace('class=darkgrey', 'class=red', $msg.'</span>');
				$uf_ko++;
			}
			else
			{
				// Copy file
				$uf_new=$GLOBALS['tmppath'].$GLOBALS['fxpglobals']['client'].'-'.$GLOBALS['fxpglobals']['user'].'-'.$GLOBALS['lts'].'-'.date('YmdHis').'-'.substr('00'.$fc,-3).'.tmp';
				$ctxt='Copy ['.$uf_tmp.'] to: ['.$uf_new.'] ... ';
				$result=@copy($uf_tmp, $uf_new);
				if($result !== true)	// ERROR
				{
uploadFilesDebug($ctxt.'ERROR #'.$result);
					$msg .= '<span class="red">';
					if($GLOBALS['fxpglobals']['lang'] == 1)
						$msg .= 'Die temporäre Datei konnte nicht in den Ziel-Ordner kopiert werden!';
					else
						$msg .= 'Failed to copy temporary file into destination folder!';
					$msg=str_replace('class=darkgrey', 'class=red', $msg.'</span>');
					$uf_ko++;
				}
				else					// SUCCESS
				{
uploadFilesDebug($ctxt.'SUCCESS');
					$v[$fc]['new_name']=$uf_new;
					$_FILES[$uid]['new_name']=$uf_new;

					$err_line=0;

					// Handle person or material picture upload?
					$hpmu=0;
					if(($utype == 'pic') && strlen($uparam))
					{
						$hpmu=-1;
						// PER|120|3|18471|...
						$upma=explode('|',$uparam);
uploadFilesDebug("\$upma:\r\n".print_r($upma,true));
						if(sizeof($upma) > 3)
						{
							$pm=strtoupper(substr($upma[0],0,3));	// PER or MAT
							$ci=(int)$upma[1];						// Client ID
							$am=(int)$upma[2];						// Action
							$id=(int)$upma[3];						// Person or Material ID
uploadFilesDebug('$pm='.$pm.', $ci='.$ci.', $am='.$am.', $id='.$id);

							if((($pm == 'PER') || ($pm == 'MAT')) && ($ci > 0) && ($am == FXP_CHANGE) && ($id > 0))
							{
								$hpmu=1;
								$umsg=uploadPicture($pm, $ci, $am, $id, array(), $uid);
uploadFilesDebug('$umsg='.$umsg);
								$uma=explode('|', $umsg);
								$msg .= $uma[1];
								if(substr($uma[0],0,1) == 'E')
									$err_line=__LINE__;
							}
						}
					}
uploadFilesDebug('$hpmu='.$hpmu);


					// Handle document upload?
					$hdu=0;
					if(($utype == 'doct') && strlen($uparam))
						$hdu=-1;
uploadFilesDebug('$hdu='.$hdu);

					if($hpmu < 0)
					{
						if($GLOBALS['fxpglobals']['lang'] == 1)
							$msg .= 'Tempor&auml;rer Bildupload erfolgreich.';
						else
							$msg .= 'Temporary upload successfull.';
					}
					else if($hdu < 0)
					{
						if($GLOBALS['fxpglobals']['lang'] == 1)
							$msg .= 'Tempor&auml;rer Dokumentenupload erfolgreich.';
						else
							$msg .= 'Temporary document upload successfull.';
					}
					else if(!$hpmu && !$hdu)
					{
						if($GLOBALS['fxpglobals']['lang'] == 1)
							$msg .= 'Upload erfolgreich.';
						else
							$msg .= 'Upload successfull.';
					}

					if(!$err_line && ($GLOBALS['fxpglobals']['tr'] == 110)) // Document Manager
					{
						// We need dms class
						if(!class_exists('fxpDoc'))
						{
							$GLOBALS['__includer']['dms']=true;
							require('includer.inc');
						}
						if(!class_exists('fxpDoc'))
						{
							$err_line=__LINE__;
							if($GLOBALS['fxpglobals']['lang'] == 1)
								$msg .= 'DMS-Klasse nicht verfügbar!';
							else
								$msg .= 'DMS-Class not available!';
						}
						else
						{
							// Create or change document
							$doc=new fxpDoc();

							ob_start();
							$em=explode('|', $doc->uploadDocument($seid, $v[$fc]));
							ob_end_clean();
							if(substr($em[0],0,1) == 'E')
								$err_line=__LINE__;
							$msg .= '<br />'.trim(str_replace('</span>','',$em[2]));
						}
					}
					else if(!$err_line && ($GLOBALS['fxpglobals']['tr'] == 217)) // Formular Design
					{
						// Check for needed POST variables
						$ktyp='';
						if(isset($_POST['upl_ktyp']))
							$ktyp=(int)$_POST['upl_ktyp'];
						if(!strlen($ktyp))
						{
							$err_line=__LINE__;
							$msg .= '<br /><span class="red">'.meldung(2033, false, '<b class="red">kt</b>');	// _POST parameter error ($var1)!
						}

						$mordner='';
						if(!$err_line)
						{
							if(isset($_POST['upl_mordner']))
								$mordner=(int)$_POST['upl_mordner'];
							if(!strlen($mordner))
							{
								$err_line=__LINE__;
								$msg .= '<br /><span class="red">'.meldung(2033, false, '<b class="red">mo</b>');	// _POST parameter error ($var1)!
							}
						}

						// Check for client folder
						$path=$mordner.'/';
						if(!$err_line)
						{
							if(!is_dir($path))
							{
								if(!@mkdir($path, 0777))
								{
									$err_line=__LINE__;
									$msg .= '<br /><span class="red">'.meldung(2034, false);	// The <b>Client Directory</b> could not be created!
								}
								else
								{
									@chmod($path, 0777);
									@chown($path, 'nobody');
								}
							}
						}

						// Get files
						$files=0;
						if(!$err_line)
						{
							$basedir=$mordner;

							if($ktyp == 2018)
							{
								$prefix='email_';
								$maxfiles=4;
							}
							else
							{
								$prefix='gfcfg_';
								$maxfiles=8;
							}

							if(@is_dir($basedir))
							{
								$dh=opendir($basedir);
								if($dh)
								{
									while(($fname=readdir($dh)) !== false)
									{
										$filename	= $basedir.'/'.$fname;
										$ftype		= filetype($filename);
										$suffix		= substr($fname, -4);

										if(($ftype == 'file') && (substr($fname, 0, 6) == $prefix) && (($suffix == '.gif') || ($suffix == '.jpg') || ($suffix == '.png')))
											$files++;
									}
								}
							}

							if($files >= $maxfiles)
							{
								$err_line=__LINE__;
								$msg .= '<br /><span class="red">'.meldung(2036, false, '<b class="red">'.$maxfiles.'</b>');	// Es können nicht mehr als [$var1] Bilder hochgeladen werden! Bitte löschen Sie zuerst ein anderes Bild.
							}
						}

						// Copy uploaded file
						if(!$err_line)
						{
							$suffix=substr(strtolower($uf_name), -4);
							if(($suffix != '.gif') && ($suffix != '.jpg') && ($suffix != '.png'))
							{
								$err_line=__LINE__;
								$msg .= '<br /><span class="red">'.meldung(2037, false);	// Unbekanntes Bildformat. Nur [<b>.gif</b>], [<b>.jpg</b>] und [<b>.png</b>] sind erlaubt!
							}
							else
							{
								if($ktyp == 2018)
									$prefix='email_';
								else
									$prefix='gfcfg_';

								$picname =convertFilename(substr($uf_name, 0, -4));
								$filename=$path.$prefix.$picname.$suffix;

								if(file_exists($filename))
									unlink($filename);

								$success=@copy($uf_tmp, $filename);
								if(!$success)
								{
									$err_line=__LINE__;
									$msg .= '<br /><span class="red">'.meldung(2038, false);	// Das hochgeladene Bild konnte <b>nicht</b> ins Zielverzeichnis kopiert werden!
								}
								else if(!file_exists($filename))
								{
									$err_line=__LINE__;
									$msg .= '<br /><span class="red">'.meldung(2039, false);	// Das <b>kopierte Bild</b> wurde nicht gefunden!
								}
								else
								{
									@chmod($filename, 0777);
									@chown($filename, 'nobody');
								}
							}
						}
					}
					else if(!$err_line && ($utype == 'pic'))
					{
						// Check picture format and get size
						// - 0: Width
						// - 1: Height
						// - 2: Type (IMAGETYPE_...): 1=GIF, 2=JPG, 3=PNG, 4=SWF, 5=PSD, 6=BMP, 7=TIFF(intel byte order), 8=TIFF(motorola byte order), 9=JPC, 10=JP2, 11=JPX, 12=JB2, 13=SWC, 14=IFF, 15=WBMP, 16=XBM
						$ims=GetImageSize($uf_new);
						if(fxIsArray($ims))
						{
							$uf_width=(int)$ims[0];
							$uf_height=(int)$ims[1];
							if(($ims[2] != 1) && ($ims[2] != 2) && ($ims[2] != 3) && ($ims[2] != 6))
							{
								$err_line=__LINE__;
								$msg .= '<br /><span class="red">'.meldung(2037, false);	// Unbekanntes Bildformat. Nur [<b>.png</b>], [<b>.jpg</b>] ,[ <b>.bmp</b>] und [<b>.gif</b>] sind erlaubt!
							}
							else if($hpmu < 0)
							{
								if($GLOBALS['fxpglobals']['lang'] == 1)
									$msg .= '<br /><br /><table><tr><td><i class=grey>&rarr;</i>&nbsp;</td><td><i class=grey>Zur &Uuml;bernahme des Bildes muss der Datensatz erst<br />noch gespeichert werden!</i></td></tr></table>';
								else
									$msg .= '<br /><br /><table><tr><td><i class=grey>&rarr;</i>&nbsp;</td><td><i class=grey>To transfer the picture the dataset must still be<br />saved first!</i></td></tr></table>';
							}
uploadFilesDebug('Uploaded picture: width='.$uf_width.', height='.$uf_height.', type='.$ims[2]);
						}
					}
					else if(!$err_line && ($utype == 'doc'))
					{
						$ims=GetImageSize($uf_new);
						if(fxIsArray($ims))
						{
							$uf_width=(int)$ims[0];
							$uf_height=(int)$ims[1];
						}
						if($hdu < 0)
						{
							if($GLOBALS['fxpglobals']['lang'] == 1)
								$msg .= '<br /><br /><table><tr><td><i class=grey>&rarr;</i>&nbsp;</td><td><i class=grey>Zur &Uuml;bernahme des Dokuments muss der Datensatz erst<br />noch gespeichert werden!</i></td></tr></table>';
							else
								$msg .= '<br /><br /><table><tr><td><i class=grey>&rarr;</i>&nbsp;</td><td><i class=grey>To transfer the document the dataset must still be<br />saved first!</i></td></tr></table>';
						}
					}

					if($err_line)
					{
						$msg=str_replace('class=darkgrey', 'class=red', $msg.'</span>');
						$uf_ko++;
					}
					else
					{
						$msg=str_replace('class=darkgrey', 'class=green', $msg);
						$uf_ok++;
						if(strlen($uf_ea['name']))
						{
							$uf_ea['name'] .= "^";
							$uf_ea['tmp']  .= "^";
							$uf_ea['new']  .= "^";
							$uf_ea['type'] .= "^";
							$uf_ea['size'] .= "^";
							$uf_ea['icon'] .= "^";
							$uf_ea['width'] .= "^";
							$uf_ea['height'] .= "^";
						}
						$uf_ea['name'] .= $uf_name;
						$uf_ea['tmp']  .= $uf_tmp;
						$uf_ea['new']  .= $uf_new;
						$uf_ea['type'] .= $uf_type;
						$uf_ea['size'] .= $uf_size;
						$uf_ea['icon'] .= $uf_icon;
						$uf_ea['width'] .= $uf_width;
						$uf_ea['height'] .= $uf_height;
					}
				}
			}
		}

		if(strlen($msg))
			$msg .= '</div>';

		// Save field
//uploadFilesDebug('sizeof($v) = '.sizeof($v));
		if(sizeof($v))
		{
			if($multi)
				$val=$v;
			else
				$val=$v[0];

			if(($uid != 'uploadfile') && ($uid != 'checkinfile'))
			{
uploadFilesDebug("\$val:\r\n".print_r($val,true));
				$GLOBALS['_form']='';
				fxSession('t','l');
				$GLOBALS['_form']=$frm;
				fxSession('t','w',$uid,$val);
uploadFilesDebug('Write session for transaction ['.$GLOBALS['fxpglobals']['tr'].']: $frm="'.$frm.'", $uid='.$uid.', $val='.$val);

				// Save global
				fxSession('g','p','upload',$val);
				fxSession('g','s');
uploadFilesDebug('Write session for global var [upload]: $val='.$val);
			}
		}

		// Return message string
		if($uf_ko < 0)
			$rtxt='CANCEL';
		else if($uf_ko > 0)
			$rtxt='ERROR';
		else
			$rtxt='OK';
		$rtxt .= '|'.$msg.'|'.$uf_ea['name'].'|'.$uf_ea['tmp'].'|'.$uf_ea['new'].'|'.$uf_ea['type'].'|'.$uf_ea['size'].'|'.$uf_ea['icon'].'|'.$uf_ea['width'].'|'.$uf_ea['height'];
	}
}

// Return text
uploadFilesDebug($rtxt);
echo($rtxt);

if($html)
{
	echo('</body>');
	echo('</html>');
}


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * Write debug log entry into file "_fx_upload_debug.txt" in the TMP-subdirectory
 *
 * @param string $msg - Optional parameter (default = ''): Debug entry to be written into the file
 */
function uploadFilesDebug($msg='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

return;

	$content ="________________________________________\r\n".date('Y-m-d H:i:s',time())."\r\n========================================\r\n";
	if($msg == '*')
		$content .= "uid=".$GLOBALS['uid'].", frm=".$GLOBALS['frm'].", utype=".$GLOBALS['utype'].", uparam=".$GLOBALS['uparam'].", html=".$GLOBALS['html']." -- spath=".$GLOBALS['spath'].", seid=".$GLOBALS['seid']."\r\n\r\n";
	if(strlen($msg) && ($msg != '*'))
		$content .= $msg;
	else
		$content .= print_r($_FILES,true);
	$content .= "\r\n\r\n";

	$fname=$GLOBALS['tmppath'].'_fx_upload_debug.txt';
	if($msg == '*')
		$clr=true;
	else
		$clr=false;
	fxSave($fname, $content, $clr);
}
?>