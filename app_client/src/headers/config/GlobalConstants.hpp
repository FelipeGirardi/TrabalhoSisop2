/**
 * Defines global constants.
 *
 * \author Marlize Ramos
 * \author Renan Kummer
 */
#pragma once

enum GlobalConstant
{
    PROFILE_MIN_SIZE = 4 + 1,
    PROFILE_MAX_SIZE = 20 + 1,
    TWEET_MAX_SIZE = 128,
    INPUT_MAX_SIZE = 4 + 1 + 128   // SEND tweet with up to 128 characters
};

enum AsciiCode
{
    END_OF_TRANSMISSION = 4
};
