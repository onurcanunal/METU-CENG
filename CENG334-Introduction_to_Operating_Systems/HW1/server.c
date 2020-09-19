#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/wait.h>
#include "message.h"
#include "logging.h"

#define PIPE(fd) socketpair(AF_UNIX, SOCK_STREAM, PF_UNIX, fd)

int main(){	

	int startingBid;
	int currentHighestBid;
	int minimumIncrement;
	int numberOfBidders;
	int i, j;
	char *bidderExecutable;
	int numberOfArguments;

	scanf("%d %d %d", &startingBid, &minimumIncrement, &numberOfBidders);

	currentHighestBid = startingBid;
	pid_t pid[numberOfBidders];
	char **bidderArguments[numberOfBidders];

	int fd[numberOfBidders][2];
	
	for(i=0; i<numberOfBidders; i++){
		bidderExecutable = (char *) malloc(100 * sizeof(char));
		scanf("%s %d", bidderExecutable, &numberOfArguments);
		numberOfArguments = numberOfArguments + 2;
		bidderArguments[i] = (char **) malloc(numberOfArguments * sizeof(char*));
		char *argumentArray[numberOfArguments];
		bidderArguments[i][0] = (char *) malloc(100 * sizeof(char));
		bidderArguments[i][0] = bidderExecutable;

		for(j=1; j<numberOfArguments-1; j++){
			bidderArguments[i][j] = (char *) malloc(100 * sizeof(char));
			scanf("%s", bidderArguments[i][j]);
		}
		bidderArguments[i][j] = (char *) malloc(sizeof(NULL));
		bidderArguments[i][j] = NULL;

		PIPE(fd[i]);
	}
	
	for(i=0; i<numberOfBidders; i++){
		if((pid[i] = fork()) == 0){
			dup2(fd[i][1], 0);
			dup2(fd[i][1], 1);
			close(fd[i][1]);
			close(fd[i][0]);
			execv(bidderArguments[i][0], bidderArguments[i]);
		}
		close(fd[i][1]);
	}

	struct pollfd pfd[numberOfBidders];
	
	for(i=0; i<numberOfBidders; i++){
		pfd[i].fd = fd[i][0];
		pfd[i].events = POLLIN;
		pfd[i].revents = 0;
	}

	cm clientMessage;
	int clientMessageSize = sizeof(cm);
	ii clientOutput;
	sm serverMessage;
	int serverMessageSize = sizeof(sm);
	oi serverOutput;
	cei startInfo;
	bi resultInfo;
    wi winnerInfo;
	int minDelay = 0;
	int start = 1;
	int bid;
	int remainingBidderCount = numberOfBidders;
	int winnerId;
	int currenthighestBidder;
	int winningBid;
	int statusArray[numberOfBidders];
	int terminationStatus;

	while(remainingBidderCount){
		for(i=0; i<numberOfBidders; i++){
			pfd[i].revents = 0;
		}
		poll(pfd, numberOfBidders, minDelay);
		for(i=0; i<numberOfBidders; i++){
			if(pfd[i].revents && POLLIN){
				if(read(pfd[i].fd, &clientMessage, clientMessageSize)){
					clientOutput.type = clientMessage.message_id;
					clientOutput.pid = pid[i];
					clientOutput.info = clientMessage.params;
					print_input(&clientOutput, i);

					if(clientMessage.message_id == CLIENT_CONNECT){
						if(start){
							minDelay = clientMessage.params.delay;
							start = 0;
						}
						else if(clientMessage.params.delay < minDelay){
							minDelay = clientMessage.params.delay;
						}
						serverMessage.message_id = SERVER_CONNECTION_ESTABLISHED;
						startInfo.client_id = i;
						startInfo.starting_bid = startingBid;
						startInfo.current_bid = currentHighestBid;
						startInfo.minimum_increment = minimumIncrement;
						serverMessage.params.start_info = startInfo;
						serverOutput.type = SERVER_CONNECTION_ESTABLISHED;
						write(fd[i][0], &serverMessage, serverMessageSize);
						serverOutput.pid = pid[i];
						serverOutput.info = serverMessage.params;
						print_output(&serverOutput, i);
					}

					else if(clientMessage.message_id == CLIENT_BID){
						bid = clientMessage.params.bid;

						// Lower than starting bid
						if(bid < startingBid){
							resultInfo.result = BID_LOWER_THAN_STARTING_BID;
						}
						// Lower than current
						else if(bid < currentHighestBid){
							resultInfo.result = BID_LOWER_THAN_CURRENT;
						}
						// Increment lower than minimum
						else if(bid - currentHighestBid < minimumIncrement){
							resultInfo.result = BID_INCREMENT_LOWER_THAN_MINIMUM;
						}
						// Accept otherwise
						else{
							resultInfo.result = BID_ACCEPTED;
							currentHighestBid = bid;
							currenthighestBidder = i;
						}
						resultInfo.current_bid = currentHighestBid;
						serverMessage.message_id = SERVER_BID_RESULT;
						serverMessage.params.result_info = resultInfo;
						serverOutput.type = SERVER_BID_RESULT;
						write(fd[i][0], &serverMessage, serverMessageSize);
						serverOutput.pid = pid[i];
						serverOutput.info = serverMessage.params;
						print_output(&serverOutput, i);

					}
					else if(clientMessage.message_id == CLIENT_FINISHED){
						remainingBidderCount--;
						statusArray[i] = clientMessage.params.status;
					}
					
				}
				else{
					pfd[i].fd = -1;
				}
			}
		}
	}
	winnerId = currenthighestBidder;
	winningBid = currentHighestBid;
	print_server_finished(winnerId, winningBid);
	winnerInfo.winner_id = winnerId;
	winnerInfo.winning_bid = winningBid;
	serverMessage.message_id = SERVER_AUCTION_FINISHED;
	serverMessage.params.winner_info = winnerInfo;
	
	for(i=0; i<numberOfBidders; i++){
		write(fd[i][0], &serverMessage, serverMessageSize);
		serverOutput.type = SERVER_AUCTION_FINISHED;
		serverOutput.pid = pid[i];
		serverOutput.info = serverMessage.params;
		print_output(&serverOutput, i);
	}
	
	for(i=0; i<numberOfBidders; i++){
		waitpid(pid[i], &terminationStatus, 0);
		print_client_finished(i, terminationStatus, (terminationStatus == statusArray[i]));
		close(fd[i][0]);
	}
	return 0;
}