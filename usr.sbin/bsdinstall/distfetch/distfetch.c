/*-
 * Copyright (c) 2011 Nathan Whitehorn
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#include <sys/param.h>
#include <stdio.h>
#include <errno.h>
#include <fetch.h>
#include <dialog.h>

static int fetch_files(int nfiles, char **urls);

int
main(void)
{
	char *diststring = strdup(getenv("DISTRIBUTIONS"));
	char **urls;
	int i, nfetched, ndists = 0;
	for (i = 0; diststring[i] != 0; i++)
		if (isspace(diststring[i]) && !isspace(diststring[i+1]))
			ndists++;
	ndists++; /* Last one */

	urls = calloc(ndists, sizeof(const char *));
	for (i = 0; i < ndists; i++) {
		urls[i] = malloc(PATH_MAX);
		sprintf(urls[i], "%s/%s", getenv("BSDINSTALL_DISTSITE"),
		    strsep(&diststring, " \t"));
	}

	chdir(getenv("BSDINSTALL_DISTDIR"));
	nfetched = fetch_files(ndists, urls);

	free(diststring);
	for (i = 0; i < ndists; i++) 
		free(urls[i]);
	free(urls);

	return ((nfetched == ndists) ? 0 : 1);
}

static int
fetch_files(int nfiles, char **urls)
{
	const char **items;
	FILE *fetch_out, *file_out;
	struct url_stat ustat;
	off_t total_bytes, current_bytes, fsize;
	char status[8];
	char errormsg[512];
	uint8_t block[4096];
	size_t chunk;
	int i, progress, last_progress;
	int nsuccess = 0; /* Number of files successfully downloaded */

	progress = 0;
	
	/* Make the transfer list for dialog */
	items = calloc(sizeof(char *), nfiles * 2);
	for (i = 0; i < nfiles; i++) {
		items[i*2] = strrchr(urls[i], '/');
		if (items[i*2] != NULL)
			items[i*2]++;
		else
			items[i*2] = urls[i];
		items[i*2 + 1] = "Pending";
	}

	init_dialog(stdin, stdout);
	dialog_vars.backtitle = __DECONST(char *, "FreeBSD Installer");
	dlg_put_backtitle();

	dialog_msgbox("", "Connecting to server.\nPlease wait...", 0, 0, FALSE);

	/* Try to stat all the files */
	total_bytes = 0;
	for (i = 0; i < nfiles; i++) {
		if (fetchStatURL(urls[i], &ustat, "") == 0 && ustat.size > 0)
			total_bytes += ustat.size;
	}

	current_bytes = 0;
	for (i = 0; i < nfiles; i++) {
		last_progress = progress;
		if (total_bytes == 0)
			progress = (i*100)/nfiles;

		fetchLastErrCode = 0;
		fetch_out = fetchXGetURL(urls[i], &ustat, "");
		if (fetch_out == NULL) {
			snprintf(errormsg, sizeof(errormsg),
			    "Error while fetching %s: %s\n", urls[i],
			    fetchLastErrString);
			items[i*2 + 1] = "Failed";
			dialog_msgbox("Fetch Error", errormsg, 0, 0,
			    TRUE);
			continue;
		}

		items[i*2 + 1] = "In Progress";
		fsize = 0;
		file_out = fopen(items[i*2], "w+");
		if (file_out == NULL) {
			snprintf(errormsg, sizeof(errormsg),
			    "Error while fetching %s: %s\n",
			    urls[i], strerror(errno));
			items[i*2 + 1] = "Failed";
			dialog_msgbox("Fetch Error", errormsg, 0, 0,
			    TRUE);
			fclose(fetch_out);
			continue;
		}

		while ((chunk = fread(block, 1, sizeof(block), fetch_out))
		    > 0) {
			if (fwrite(block, 1, chunk, file_out) < chunk)
				break;

			current_bytes += chunk;
			fsize += chunk;
	
			if (total_bytes > 0) {
				last_progress = progress;
				progress = (current_bytes*100)/total_bytes; 
			}

			if (ustat.size > 0) {
				sprintf(status, "-%jd", (fsize*100)/ustat.size);
				items[i*2 + 1] = status;
			}

			if (progress > last_progress)
				dialog_mixedgauge("Fetching Distribution",
				    "Fetching distribution files...", 0, 0,
				    progress, nfiles,
				    __DECONST(char **, items));
		}

		if (ustat.size > 0 && fsize < ustat.size) {
			if (fetchLastErrCode == 0) 
				snprintf(errormsg, sizeof(errormsg),
				    "Error while fetching %s: %s\n",
				    urls[i], strerror(errno));
			else
				snprintf(errormsg, sizeof(errormsg),
				    "Error while fetching %s: %s\n",
				    urls[i], fetchLastErrString);
			items[i*2 + 1] = "Failed";
			dialog_msgbox("Fetch Error", errormsg, 0, 0,
				    TRUE);
		} else {
			items[i*2 + 1] = "Done";
			nsuccess++;
		}

		fclose(fetch_out);
		fclose(file_out);
	}
	end_dialog();

	free(items);
	return (nsuccess);
}
