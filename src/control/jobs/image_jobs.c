/*
    This file is part of darktable,
    copyright (c) 2010 Henrik Andersson.

    darktable is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    darktable is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with darktable.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "common/darktable.h"
#include "common/image_cache.h"
#include "control/jobs/image_jobs.h"

void dt_image_load_job_init(dt_job_t *job, int32_t id, dt_image_buffer_t mip)
{
  dt_control_job_init(job, "load image %d mip %d", id, mip);
  job->execute = &dt_image_load_job_run;
  dt_image_load_t *t = (dt_image_load_t *)job->param;
  t->imgid = id;
  t->mip = mip;
}

int32_t dt_image_load_job_run(dt_job_t *job)
{
  dt_image_load_t *t = (dt_image_load_t *)job->param;
  dt_image_t *img = dt_image_cache_get(t->imgid, 'r');
  if(!img) return 1;
  
  char message[512]={0};
  snprintf(message, 512, _("loading image %s"), img->filename);
  const dt_gui_job_t *j = dt_gui_background_jobs_new( DT_JOB_SINGLE, message );
  
  int ret = dt_image_load(img, t->mip);
  // drop read lock, as this is only speculative async loading.
  if(!ret) dt_image_release(img, t->mip, 'r');
  dt_image_cache_release(img, 'r');
  dt_gui_background_jobs_set_progress( j, 1.0 );
  dt_gui_background_jobs_destroy (j);
  return 0;
}