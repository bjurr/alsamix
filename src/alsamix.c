#include <node_api.h>
#include <alsa/asoundlib.h>

void SetAlsaVolume(long volume, char *card, char *selem_name)
{
    long min, max;
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_set_playback_volume_all(elem, volume * max / 100);

    snd_mixer_close(handle);
}

long GetAlsaVolume(char *card, char *selem_name)
{
    long min, max, volume;
 
    snd_mixer_t *handle;
    snd_mixer_selem_id_t *sid;

    snd_mixer_open(&handle, 0);
    snd_mixer_attach(handle, card);
    snd_mixer_selem_register(handle, NULL, NULL);
    snd_mixer_load(handle);

    snd_mixer_selem_id_alloca(&sid);
    snd_mixer_selem_id_set_index(sid, 0);
    snd_mixer_selem_id_set_name(sid, selem_name);
    snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

    snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
    snd_mixer_selem_get_playback_volume(elem, SND_MIXER_SCHN_FRONT_LEFT, &volume);
    snd_mixer_close(handle);
 
    // Add 0.5% to the value for proper rounding up.
    return (volume + (max / 100 / 2)) * 100 / max;
}

napi_value SetVolume(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 3;
    int volume = 0;
    char card[32] = {0};
    char selem_name[32] = {0};
    napi_value argv[3];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }
    
    // Get the volume
    status = napi_get_value_int32(env, argv[0], &volume);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid volume was passed as argument");
    }
  
    // Get the card name
    status = napi_get_value_string_utf8(env, argv[1], card, 32, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid card name was passed as argument");
    }

    // And the simple element name
    status = napi_get_value_string_utf8(env, argv[2], selem_name, 32, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid simple element name was passed as argument");
    }

    SetAlsaVolume(volume, card, selem_name);
    return NULL;
}

napi_value GetVolume(napi_env env, napi_callback_info info)
{
    napi_status status;
    size_t argc = 2;
    long volume = 0;
    char card[32] = {0};
    char selem_name[32] = {0};
    napi_value argv[2];
    status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Failed to parse arguments");
    }
    
    // Get the card name
    status = napi_get_value_string_utf8(env, argv[0], card, 32, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid card name was passed as argument");
    }

    // And the simple element name
    status = napi_get_value_string_utf8(env, argv[1], selem_name, 32, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Invalid simple element name was passed as argument");
    }

    volume = GetAlsaVolume(card, selem_name);
    napi_value result;
    status = napi_create_int32(env, volume, &result);

    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to create return value");
    }

    return result;
}

napi_value Init(napi_env env, napi_value exports) {
    napi_status status;
    napi_value fn;

    status = napi_create_function(env, NULL, 0, SetVolume, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "set_volume", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    status = napi_create_function(env, NULL, 0, GetVolume, NULL, &fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to wrap native function");
    }

    status = napi_set_named_property(env, exports, "get_volume", fn);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Unable to populate exports");
    }

    return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)