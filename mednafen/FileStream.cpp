// TODO/WIP

/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */


#include "mednafen.h"
#include "Stream.h"
#include "FileStream.h"

#include <stdarg.h>
#include <string.h>

FileStream::FileStream(const char *path, const int mode): OpenedMode(mode)
{
   fp = retro_fopen(path, (mode == MODE_WRITE) ? RFILE_MODE_WRITE : RFILE_MODE_READ, -1);

   if (!fp)
   {
      ErrnoHolder ene(errno);

      throw(MDFN_Error(ene.Errno(), "Error opening file %s", ene.StrError()));
   }

   original_path = strdup(path);
}

FileStream::~FileStream()
{
   if (original_path)
      free(original_path);
   original_path = NULL;
}

uint64_t FileStream::attributes(void)
{
   uint64_t ret = ATTRIBUTE_SEEKABLE;

   switch(OpenedMode)
   {
      case MODE_READ:
         ret |= ATTRIBUTE_READABLE;
         break;
      case MODE_WRITE_SAFE:
      case MODE_WRITE:
         ret |= ATTRIBUTE_WRITEABLE;
         break;
   }

   return ret;
}

uint64_t FileStream::read(void *data, uint64_t count, bool error_on_eos)
{
   if (!fp)
      return 0;
   return retro_fread(fp, data, count);
}

void FileStream::write(const void *data, uint64_t count)
{
   if (!fp)
      return;
   retro_fwrite(fp, data, count);
}

void FileStream::seek(int64_t offset, int whence)
{
   if (!fp)
      return;
   retro_fseek(fp, offset, whence);
}

int64_t FileStream::tell(void)
{
   if (!fp)
      return -1;
   return retro_ftell(fp);
}

int64_t FileStream::size(void)
{
   if (!original_path)
      return -1;

   return path_get_size(original_path);
}

void FileStream::close(void)
{
   if (!fp)
      return;
   retro_fclose(fp);
}
