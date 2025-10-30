#ifndef SOUND_MODULE
#define SOUND_MODULE

#include <AL/alut.h>

class sound
{
    char **sounds;
    int soundCount;
    ALuint buffer;
    ALuint source;
    ALenum error;
    ALint status;
public:
    sound(char **filenames, int fileCount);
    const void playSound (const char *fileName);
    const bool IsSoundPLaying();
    ~sound();
};

sound::sound(char **filenames, int fileCount)
{
    sounds = new (char*);
    sounds = filenames;
    soundCount = fileCount;
}

const void sound::playSound (const char *fileName)
{
  /* Create an AL buffer from the given sound file. */
  buffer = alutCreateBufferFromFile (fileName);
  if (buffer == AL_NONE)
  {
    error = alutGetError ();
    fprintf (stderr, "Error loading file: '%s'\n",
              alutGetErrorString (error));
    alutExit ();
    exit (EXIT_FAILURE);
  }

  /* Generate a single source, attach the buffer to it and start playing. */
  alGenSources (1, &source);
  alSourcei (source, AL_BUFFER, buffer);
  alSourcePlay (source);

  /* Normally nothing should go wrong above, but one never knows... */
  error = alGetError ();
  if (error != ALUT_ERROR_NO_ERROR)
  {
    fprintf (stderr, "%s\n", alGetString (error));
    alutExit ();
    exit (EXIT_FAILURE);
  }
}

const bool sound::IsSoundPLaying()
{
    alGetSourcei (source, AL_SOURCE_STATE, &status);
    if (status == AL_PLAYING)
    {
        return true;
    }
    return false;
}

sound::~sound()
{
    delete(sounds);
    if (!alutExit())
    {
      ALenum error = alutGetError ();
      fprintf (stderr, "%s\n", alutGetErrorString (error));
      exit (EXIT_FAILURE);
    }
}

#endif