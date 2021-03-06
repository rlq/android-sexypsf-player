/*  Copyright (C) 2009 The android-sexypsf Open Source Project
 *
 *  This file is part of android-sexypsf.
 *  android-sexypsf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  android-sexypsf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with android-sexypsf.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <jni.h>

#define DEBUG_LEVEL 0
//#define DEBUG_DUMP_PCM
#include "sexypsf_android.h"

#ifdef DEBUG_DUMP_PCM
  #include <stdio.h>
  // dump the decoded audio data
  static FILE* dump_file;
#endif

static char sexypsf_filename[1024];
//jstring to char*
//!!!It uses a 1K buffer to store the string, so use it only once!!!
static char* jstringTostring(JNIEnv* env, jstring jstr)
{
       char* rtn = sexypsf_filename;
       jclass clsstring = (*env)->FindClass(env, "java/lang/String");
       jstring strencode = (*env)->NewStringUTF(env, "utf-8");
       jmethodID mid = (*env)->GetMethodID(env, clsstring, "getBytes", "(Ljava/lang/String;)[B");
       jbyteArray barr= (jbyteArray)(*env)->CallObjectMethod(env, jstr, mid, strencode);
       jsize alen = (*env)->GetArrayLength(env, barr);
       jbyte* ba = (*env)->GetByteArrayElements(env, barr, JNI_FALSE);
       if (alen > 0 && alen <1024)
       {
           memcpy(rtn, ba, alen);
           rtn[alen] = 0;
       }
       (*env)->ReleaseByteArrayElements(env, barr, ba, 0);
       return rtn;
}

/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfopen

DESCRIPTION: Sexypsf wrapper to call function psf_open

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object
   filename - file name string, NULL terminated

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==================================================================================================*/
jboolean Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfopen( JNIEnv* env,
                                             jobject thiz,
                                             jstring filename,
                                             jint psftype)
{
	PSF_TYPE type = -1;
    const char* name = jstringTostring(env, filename);
#ifdef DEBUG_DUMP_PCM
    char* dump_file_name = (char*)malloc(strlen(name)+5);
    if (dump_file_name != NULL) {
    	strcpy(dump_file_name, name);
    	strcat(dump_file_name, ".dmp");
        dump_file = fopen(dump_file_name, "wb");
        if (dump_file) {
            debug_printf("Opened dump file %s\n", dump_file_name);
        }
        else {
            debug_printf("Open dump file failure %s\n", dump_file_name);
        }
        free(dump_file_name);
    }
#endif
	if (psftype == 1) {
		type = TYPE_PSF;
	}
	else if (psftype == 2) {
		type = TYPE_PSF2;
	}
    return psf_open(name, type);
}


/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfplay

DESCRIPTION: Sexypsf wrapper to call function psf_play

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   This function will not return until the playback is done or psf_stop is called.

==================================================================================================*/
void Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfplay( JNIEnv* env,
                                             jobject thiz)
{
    psf_play();
}


/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfpause

DESCRIPTION: Sexypsf wrapper to call function psf_pause

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object
   pause    - Indicate if it's pause or resume

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==================================================================================================*/
void Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfpause( JNIEnv* env,
                                             jobject thiz,
                                             jboolean   pause)
{
    psf_pause(pause);
}

/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfseek

DESCRIPTION: Sexypsf wrapper to call function psf_seek

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object
   seek     - the offset of seek
   mode     - the seek mode

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==================================================================================================*/
void Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfseek( JNIEnv* env,
                                             jobject thiz,
                                             jint    seek,
                                             jint    mode)
{
    psf_seek(seek, mode);
}

/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfstop

DESCRIPTION: Sexypsf wrapper to call function psf_stop

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==================================================================================================*/
void Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfstop( JNIEnv* env,
                                             jobject thiz)
{
    psf_stop();
#ifdef DEBUG_DUMP_PCM
    if (dump_file) {
        debug_printf("Closing dump file\n");
    	fclose(dump_file);
    }
    dump_file = NULL;
#endif
}


/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfputaudiodata

DESCRIPTION: Sexypsf wrapper to put audio data to java buffer

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object
   arr      - Java byte array
   size     - size of the audio data to put

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==================================================================================================*/
jint Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfputaudiodata( JNIEnv* env,
                                             jobject thiz, jbyteArray arr, jint size)
{
    jbyte *carr;
    int ret;
    carr = (*env)->GetPrimitiveArrayCritical(env, arr, NULL);
    if (carr == NULL) {
        debug_printf("Get NULL array in JNI call, nothing to put!!!\n");
        return 0; /* exception occurred */
    }
    ret = psf_audio_putdata((uint8_t*)carr, size);
    debug_printf("sexypsfputaudiodata: req %d, got %d", size, ret);
#ifdef DEBUG_DUMP_PCM
    if (dump_file) {
        debug_printf("Dump pcm data %d\n", ret);
		fwrite(carr, ret, 1, dump_file);
    }
#endif
    (*env)->ReleasePrimitiveArrayCritical(env, arr, carr, 0);
    return ret;
}

/*==================================================================================================

FUNCTION: Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfputaudiodataindex

DESCRIPTION: Sexypsf wrapper to put audio data to java buffer

ARGUMENTS PASSED:
   env      - JNI env
   jobject  - JNI object
   arr      - Java byte array
   size     - size of the audio data to put

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==================================================================================================*/
jint Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfputaudiodataindex( JNIEnv* env,
                                      jobject thiz, jbyteArray arr, jint index, jint size)
{
    jbyte *carr;
    int ret;
    carr = (*env)->GetPrimitiveArrayCritical(env, arr, NULL);
    if (carr == NULL) {
        debug_printf("Get NULL array in JNI call, nothing to put!!!\n");
        return 0; /* exception occurred */
    }
    ret = psf_audio_putdata((uint8_t*)carr+index, size);
    debug_printf("sexypsfputaudiodataindex: req %d, got %d", size, ret);
#ifdef DEBUG_DUMP_PCM
    if (dump_file) {
        debug_printf("Dump pcm data %d\n", ret);
		fwrite(carr+index, ret, 1, dump_file);
    }
#endif
    (*env)->ReleasePrimitiveArrayCritical(env, arr, carr, 0);
    return ret;
}

jobject Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfgetpsfinfo(JNIEnv* env,
		jobject thiz, jstring filename)
{
    char* name = jstringTostring(env, filename);
    PSF_INFO *tmp;

    if((tmp=psf_getinfo(name)))
    {
    	jclass cls = NULL;
    	jmethodID constructor;
    	jvalue args[5];
    	jobject object;

    	debug_printf("get psf info success");

    	// get a reference to the class
    	cls = (*env)->FindClass(env, "com/mine/psf/sexypsf/PsfInfo");
    	if (cls == NULL) {
    		debug_printf("Failed to get class");
		psf_freeinfo(tmp);
    		return NULL;
    	}

    	// get a reference to the constructor; the name is <init>
    	// with 5 parameters, 1 int, 4 String
    	constructor = (*env)->GetMethodID(env, cls, "<init>",
    			"(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    	if (constructor == NULL) {
    		debug_printf("Failed to get the method");
		psf_freeinfo(tmp);
    		return NULL;
    	}

    	// set up the arguments; i means int, l means object
    	args[0].i = tmp->length;
    	args[1].l = (*env)->NewStringUTF(env, tmp->title);
    	args[2].l = (*env)->NewStringUTF(env, tmp->artist);
    	args[3].l = (*env)->NewStringUTF(env, tmp->game);
    	args[4].l = (*env)->NewStringUTF(env, tmp->copyright);
    	object = (*env)->NewObjectA(env, cls, constructor, args);
	psf_freeinfo(tmp);

	return object;
    }
    else {
    	debug_printf("get psf info fail");
    	return NULL;
    }
}

jint Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfgetpos(JNIEnv* env, jobject thiz)
{
	return psf_get_pos();
}


void Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfquit( JNIEnv* env,
                                             jobject thiz)
{
	sexypsf_quit();
}


void Java_com_mine_psf_sexypsf_MineSexyPsfLib_sexypsfsetinfiniteloop(JNIEnv* env,
                                             jobject thiz,
                                             jboolean loop)
{
	psf_set_infinite_loop(loop);
}
