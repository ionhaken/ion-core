/*
 * Copyright 2023 Markus Haikonen, Ionhaken
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once
#include <atomic>
#include <ion/jobs/BaseJob.h>
#include <ion/jobs/JobQueue.h>

namespace ion
{
class JobScheduler;
class JobGroup
{
public:
	JobGroup();
	~JobGroup();

	bool IsEmpty() const { return mNumJobsAvailable == 0; };

	void PushJob(ion::BaseJob& job)
	{
		mNumJobsAvailable++;
		mQueue.PushTaskAndWakeUp(JobWork(&job));
	}

	bool Work(ion::JobScheduler& js);

	void Finalize();

private:
	JobQueueStats mStats;
	ion::JobQueueSingleOwner mQueue;
	std::atomic<UInt> mNumJobsAvailable;
};
}  // namespace ion
