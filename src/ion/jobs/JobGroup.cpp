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
#include <ion/jobs/JobGroup.h>
#include <ion/concurrency/Thread.h>
#include <ion/jobs/JobScheduler.h>
#include <ion/jobs/JobQueue.inl>

namespace ion
{
ION_CODE_SECTION(".jobs")
JobGroup::JobGroup() : mNumJobsAvailable(1)
{
}
ION_SECTION_END

ION_CODE_SECTION(".jobs")
JobGroup::~JobGroup()
{
}
ION_SECTION_END

ION_CODE_SECTION(".jobs")
bool JobGroup::Work(ion::JobScheduler& js)
{
	if (mQueue.Run() == JobQueueStatus::Empty)
	{
		ION_ASSERT(ion::Thread::GetQueueIndex() != ion::Thread::NoQueueIndex, "Not supported. See WaitableJob::Wait()");
		if ION_LIKELY (js.GetPool().GetWorkerCount() > 0)
		{
			js.GetPool().AddCompanionWorker();
			// Wait until other thread does JobGroup::Finalize()
			mQueue.Wait(mStats);
			js.GetPool().RemoveCompanionWorker();
		}
		else
		{
			// Do JobGroup::Finalize() by myself
			js.GetPool().WorkOnMainThreadNoBlock();
		}
	}
	else
	{
		mNumJobsAvailable--;
	}
	return mNumJobsAvailable != 0;
}
ION_SECTION_END

ION_CODE_SECTION(".jobs")
void JobGroup::Finalize()
{
	mNumJobsAvailable--;
	mQueue.WakeUpAll();
}
ION_SECTION_END

}  // namespace ion
