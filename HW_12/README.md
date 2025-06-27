# 12. Mapreduce
# MapReduce Homework

This is a task to work with a bunch of different tools. Hopes that each will find something new for themselves in this task.

You will learn (mandatory part for verification):
- Calculate the mean and variance in the map-reduce paradigm

And the bravest will also learn (not necessarily, without verification):
- Use docker images
- Work with hadoop hdfs and hadoop map reduce

## 1. Mom, I'm an analyst (mandatory part of the task, for checking, C++ will be only in this part)
Take a project template.

In the input directory, there is a dataset, which is cleared of crooked lines in advance, but if you want to complete the "task with an asterisk", then download the original dataset from kaggle, the link is there nearby.

Study the makefile, it will help you compile your code. To build the mapper, type `make bin/mapper` in the console, similarly, for the reducer. `make all` will build both files at once, and `make clean` will clean up after the work.

mapper.cpp and reducer.cpp are placeholders for your code

run_locally.sh will help you manually test your code (manual testing does not eliminate the need for unit tests)

This is just a draft of the project. You can modify any scripts if necessary, do as you wish. You can use other assembly tools.

Get to work... Study the dataset, if it is not clear which column is - read the description on Kaggle (https://www.kaggle.com/datasets/dgomonov/new-york-city-airbnb-open-data?resource=download). Today's task is to calculate a pair of statistics in the map-reduce paradigm: the average price of real estate in NY and the price variance. Write mappers and reducers for this. For each statistic, you will get your mapper+reducer pair. They should work successfully when you run run_locally.sh

If you are not sure that you calculated the values ​​correctly, use your favorite data analysis tool (matlab/R/python+pandas/etc) and check yourself. And do not neglect unit tests. The most inquisitive can try to do this work using the TDD technique (https://ru.wikipedia.org/wiki/%D0%A0%D0%B0%D0%B7%D1%80%D0%B0%D0%B1%D0%BE%D1%82%D0%BA%D0%B0_%D1%87%D0%B5%D1%80%D0%B5%D0%B7_%D1%82%D0%B5%D1%81%D1%82%D0%B8%D1%80%D0%BE%D0%B2%D0%B0%D0%BD%D0%B8%D0%B5), because studying is a great time to try new approaches.

By the end of this step, you will have two mapper+reducer pairs that can calculate the mean and variance of the price from the dataset you were given. Submit them for review along with everything you need to build and review them.

## 2. What do devops and a homeless person have in common? Both are good at containers (optional part; not for review; for those who can handle it)
So, if you don't know how to use docker yet, now is the time to learn. `man docker` and the Internet will help you (https://habr.com/ru/companies/ruvds/articles/438796/). Docker is a great tool, it's a virtual machine at the OS level, but not at the hardware level. It is used to deliver your software along with the necessary environment, and quickly deploy it on any server or on a bunch of servers at once. It will come in handy in your life. Now we will not learn how to pack our software into an image, but we will only use other people's images. This stage can be more painful for Windows users than for Unix users, this is the way.

And one more tool - docker-compose. It is useful when you need to run several docker containers in a bundle. Similar: `man docker-compose` and habr (https://habr.com/ru/companies/ruvds/articles/450312/).

Now we are ready to continue...

We take someone else's repo https://github.com/tech4242/docker-hadoop-hive-parquet, clone it, and run `docker-compose up`. We run several containers with different parts of Hadoop, the list can be viewed in `docker ps` (the first column is the container id). You can copy files from your machine to any of the running containers using `docker cp <src_file> <cont_id>:<dest_path>`. Open the console in any of the containers `docker exec -it <id> /bin/bash`.

Now our task is to make your mapper and reducer able to run in the datanode container. Copy them there, open the console, and try to run them. Most likely, the libc versions on your machine where you compiled the binaries and in the container will not match, and some other problems may pop up. We are valiantly solving problems, whoever copes with it is a good boy and will be able to move on to the next stage of the task.
- Hint: I suspect that the shortest way is to take another docker image with the same version of the operating system as in the already running containers, and with g++ installed, compile your binaries in this environment, then they will run in the target containers, i.e. you need to build and run in the same environment.
- Hint2: another way is to modify the images from this repo so that the OS version in them matches yours. In real life, you will most likely write your own Docker image configurations for your software and have two Docker images with similar configurations: one for building and testing your service in CI, the other for rolling out to production, and you will not have problems with libc versions, because you will take this into account right away.

The result of the second stage: you know how to run docker containers, and your mapper and reducer can run inside them (inside the datanode).

## 3. Opening the world of big data (optional part; not for testing; for those who can handle it)

### HDFS
1. Copy the dataset from your machine to the container with namenode - `docker cp` will help
2. Let's open the namenode console and poke around in the hdfs console interface ([https://gist.github.com/Alexflex/c8c53346b2feed4863956ab633fdd493#команды-shell](https://gist.github.com/Alexflex/c8c53346b2feed4863956ab633fdd493#%D0%BA%D0%BE%D0%BC%D0%B0%D0%BD%D0%B4%D1%8B-shell)). Once you've got the hang of it, copy the dataset from the container's file system to hdfs.

### Hadoop Map-Reduce
1. Copy your mapper and reducer from your machine to the container with the namenode. They do not need to be placed in hdfs, just in the container's FS.
2. Run the map reduce task to calculate the average price (search for Hadoop MapReduce tutorials or just follow the prompts below):
   - Hint1: You will need the hadoop-streaming.jar file. Use the command `find / -name 'hadoop-streaming*.jar'` to find where in the container the hadoop-streaming.jar is located. Run it with the command `yarn jar <path> [params]`
   - Hint2: figure out how to use it, and if you have any difficulties, you can use my script as a basis:
```
#!/usr/bin/env bash
set -x

\
HADOOP_STREAMING_JAR=/opt/hadoop-2.7.4/share/hadoop/tools/lib/hadoop-streaming-2.7.4.jar
OUT_DIR=$2

\
hdfs dfs -rm -r $OUT_DIR

yarn jar $HADOOP_STREAMING_JAR \
	-D mapreduce.job.name=$3 \  # any name for logs
	-files mapper_mean.py,reducer_mean.py \  # list of files to copy from namenode to datanode before running map reduce
    -mapper 'python3 mapper_mean.py' \  # command to run mapper on datanode node, here is an example with python, yours will be different
    -reducer 'python3 reducer_mean.py' \  # command to run reducer on datanode node
    -numReduceTasks 1 \  # number of reducers
    -input $1 \  # path to dataset in hdfs
    -output $OUT_DIR  # path where to put the result in hdfs

\
hdfs dfs -cat $OUT_DIR/part-00000

\
echo $?

\
```

Result of the third stage:
- you poked around in the hdfs console utility, learned how to write files to hdfs, and do other things with this file system;
- you launched a real mapreduce task in a real bigdata framework (Hadoop MapReduce) and it calculated the necessary statistics

**Profit! You are great! Keep it up!**
