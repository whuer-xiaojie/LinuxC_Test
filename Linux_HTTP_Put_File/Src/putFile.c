#include <stdio.h>  

#include <string.h>    

#include <curl/curl.h>


int main(int argc, char *argv[])
{
	CURL *curl;
	CURLcode res;

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	static const char buf[] = "Expect:";

	//curl_global_init(CURL_GLOBAL_ALL);

	/* Fill in the file upload field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "file",
		CURLFORM_FILE, "test123.jpg",
		CURLFORM_END);

	/* Fill in the filename field */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "name",
		CURLFORM_COPYCONTENTS, "test123.jpg",
		CURLFORM_END);

	/* Fill in the submit field too, even if this is rarely needed */
	curl_formadd(&formpost,
		&lastptr,
		CURLFORM_COPYNAME, "type",
		CURLFORM_COPYCONTENTS, "image",
		CURLFORM_END);

	curl = curl_easy_init();
	/* initalize custom header list (stating that Expect: 100-continue is not
	wanted */
	//headerlist = curl_slist_append(headerlist, buf);
	if (curl) {
		/* what URL that receives this POST */

		curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.155.196:3001/api/storage/file/upload");
		if ((argc == 2) && (!strcmp(argv[1], "noexpectheader")))
			/* only disable 100-continue header if explicitly requested */
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
			//fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);

		/* then cleanup the formpost chain */
		curl_formfree(formpost);
		/* free slist */
		//curl_slist_free_all(headerlist);
	}
	return 0;
}