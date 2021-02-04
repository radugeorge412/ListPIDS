#include <stdio.h>
#include <stdlib.h>
//opendir
#include <dirent.h>
#include <errno.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#define MAX_LINE 1000
//stiva
int MAXSIZE = 100;
int stack[1000];
int top = -1;
int isempty() {
	if (top == -1)
		return 1;
	else
		return 0;
}
int isfull() {
	if (top == MAXSIZE)
		return 1;
	else
		return 0;
}
int peek() {
	return stack[top];
}
int pop() {
	int data;
	if (!isempty()) {
		data = stack[top];
		top = top - 1;
		return data;
	}
}
int push(int data) {
	if (!isfull()) {
		top = top + 1;
		stack[top] = data;
	}
}


int main(int argc, char *argv[]) {
	int ppidvect[99999], done[99999];
	int aux, PPid;
	char *p;
	//int givPID = strtol(argv[1], &p, 10);
	push(strtol(argv[1], &p, 10)); //adaugam in stiva pid-ul primit
	DIR *dirp;
	struct dirent *dp;
	char path[PATH_MAX];
	char line[MAX_LINE], cmd[MAX_LINE], state[MAX_LINE];
	FILE *fp;
	_Bool gotName, gotState, gotPPid;



	dirp = opendir("/proc");
	if (dirp == NULL)
		errno;

	//trecem odata prin procese ptr a creea un vector de tati
	for (;;) {
		dp = readdir(dirp);
		if (dp == NULL) {
			break;
		}

		//Verificam doar foldere ptr ca ne uitam dupa procese (obligatoriu incepe cu numar)
		if (dp->d_type != DT_DIR || !isdigit((unsigned char)dp->d_name[0]))
			continue;

		snprintf(path, PATH_MAX, "/proc/%s/status", dp->d_name);

		fp = fopen(path, "r");
		if (fp == NULL)
			continue;

		gotPPid = FALSE;
		while (!gotPPid) {
			if (fgets(line, MAX_LINE, fp) == NULL)
				break;
			//extragere PPid
			if (strncmp(line, "PPid:", 5) == 0) {
				PPid = strtol(line + 5, NULL, 10);
				gotPPid = TRUE;
			}
		}
		fclose(fp);
		//adaugare tata in vector
		aux = strtol(dp->d_name, &p, 10);
		if (gotPPid)
			ppidvect[aux] = PPid;
	}


	//dirp = opendir("/proc");
	while (peek() != 0) {
		aux = pop();
		done[aux] = 1;
		for (int i = 1; i < 99999; i++) {
			if (ppidvect[i] == aux && done[i] != 1)
				push(i);
		}
		snprintf(path, PATH_MAX, "/proc/%d/status", aux);
		fp = fopen(path, "r");

		if (fp == NULL)
			continue;

		gotName = FALSE;
		gotState = FALSE;
		while (!gotName || !gotState) {
			if (fgets(line, MAX_LINE, fp) == NULL)
				break;

			if (strncmp(line, "Name:", 5) == 0) {
				for (p = line + 5; *p != '\0' && isspace((unsigned char)*p); )
					p++;
				strncpy(cmd, p, MAX_LINE - 1);
				gotName = TRUE;
			}

			if (strncmp(line, "State:", 6) == 0) {
				strncpy(state, p, MAX_LINE - 1);
				state[MAX_LINE - 1] = '\0';
				gotState = TRUE;
			}
		}

		fclose(fp);

		//Daca s-a gasit nume si uid se afisaza
		if (gotName && gotState)
			printf("%5d \t%s\t|->PPid: %d %12s", aux, cmd, ppidvect[aux], state);
	}


	return 0;
}
