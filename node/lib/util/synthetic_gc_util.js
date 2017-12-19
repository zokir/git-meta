/*
 * Copyright (c) 2017, Two Sigma Open Source
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of git-meta nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

"use strict";

const assert  = require("chai").assert;
const co = require("co");
const NodeGit = require("nodegit");
const SubmoduleUtil = require("./submodule_util");
const SubmoduleConfigUtil = require("./submodule_config_util");
const SubmoduleFetcher = require("./submodule_fetcher");
const UserError = require("./user_error");
const GitUtil = require("./git_util");

const SYNTHETIC_BRANCH_BASE = "refs/commits/";

let refExists = function(existingReferences, sha) {
    return existingReferences.has(sha);
};

/**
 * This class provides a way to list/delete synthetic refs, by following two
 * main algorithms.
 *  1) Removing refs that have persistent descendant.
 *  2) Removing refs that are older than specific threshold date and not part
 *     of the persistent descendant list.
 *
 * @SEE_ALSO: lib/synthetic_gc
 */
class SyntheticGcUtil {

    /**
    * Create a 'SyntheticGcUtil' object for synthetic refs manipulation.
    */
    constructor() {
        this.d_visited = {};
        this.d_metaVisited = {};
        this.d_simulation = true;

        // due to absense of value inequality in set
        this.d_subCommitStored = {};
    }

    /**
    * If simulation set to true, no synthetic refs are
    * actually being removed.
    * @param {Boolean}
    */
    get simulation() {
        return this.d_simulation;
    }

    set simulation(value) {
        this.d_simulation = value;
    }

    /**
    * @param {Set} visited commits
    */
    get visited() {
        return this.d_visited;
    }

    set visited(value) {
        this.d_visited = value;
    }

} // SyntheticGcUtil

/**
 * Return bare submodule repository corresponding to a specified `refHeadCommit`
 * within specified meta `repo`.
 *
 * @param {NodeGit.Repository}   repo
 * @param {NodeGit.Commit}       commit
 * @return {NodeGit.Repository}  subRepo
 */
SyntheticGcUtil.prototype.getBareSubmoduleRepo = co.wrap(
    function*(repo, subName, refHeadCommit) {

        assert.instanceOf(repo, NodeGit.Repository);
        assert.instanceOf(refHeadCommit, NodeGit.Commit);

        const fetcher = new SubmoduleFetcher(repo, refHeadCommit);

        let subUrl = yield fetcher.getSubmoduleUrl(subName);

        const metaUrl = yield GitUtil.getOriginUrl(repo);
        if (metaUrl !== null) {
            subUrl = SubmoduleConfigUtil.resolveSubmoduleUrl(metaUrl,
                                                             subUrl);
        }

        const subRepo = yield NodeGit.Repository.open(subUrl);

        return subRepo;
});

/**
 * Remove synthetic ref corresponding to specified `commit` in the specified
 * `repo`.
 *
 * @param {NodeGit.Repository}   repo
 * @param {NodeGit.Commit}       commit
 */
SyntheticGcUtil.prototype.removeSyntheticRef = co.wrap(
    function(repo, commit) {

    assert.instanceOf(repo, NodeGit.Repository);
    assert.instanceOf(commit, NodeGit.Commit);

    const synRefPath = SYNTHETIC_BRANCH_BASE + commit;

    if (this.d_simulation) {
        // following 'git clean -n'
        console.log("Would remove: " + synRefPath);
        return;
    }

    const failed = NodeGit.Reference.remove(repo, synRefPath);
    if (failed) {
        throw new UserError("Failed to remove the reference: " + synRefPath);
    }
});

/**
 * Go through the parents of `commit` of the specified `repo` and remove
 * synthetic reference recursively if they satisfy `isDeletable` and not part of
 * `existingReferences`.
 *
 * @param {NodeGit.Repository}   repo
 * @param {NodeGit.Commit}       commit
 * @param {Function}             isDeletable
 * @param {Set<String>}          existingReferences
 */

SyntheticGcUtil.prototype.recursiveSyntheticRefRemoval = co.wrap(
    function* (repo, commit, isDeletable, existingReferences) {

    assert.instanceOf(repo, NodeGit.Repository);
    assert.instanceOf(commit, NodeGit.Commit);

    if (commit.parentcount() === 0) {
        return;
    }

    const parents = yield commit.getParents(commit.parentcount());
    let thisState = this;
    yield parents.map(function *(parent) {
        if (parent.sha() in thisState.d_visited) {
            return;
        }
        thisState.d_visited[parent.sha()] = 1;

        // We are keeping track of 'existingReferences' to avoid trying to
        // delete references that do not exists. This is helpful in simulation
        // mode, since it provides a clear way to a user what actually is being
        // deleted. Also helps in debugging.
        if (isDeletable(parent) &&
                refExists(existingReferences, parent.sha())) {
            thisState.removeSyntheticRef(repo, parent);
        }
        return yield thisState.recursiveSyntheticRefRemoval(repo, parent,
                                                   isDeletable,
                                                   existingReferences);
    });

});

/**
 * Return all available synthetic refs within specified `subRepo`.
 *
 * @param {NodeGit.Repo}   subRepo
 * @return {String[]}
 */
SyntheticGcUtil.prototype.getSyntheticRefs = co.wrap(
    function*(subRepo) {

    assert.instanceOf(subRepo, NodeGit.Repository);
    let references = yield NodeGit.Reference.list(subRepo);

    references = references.filter(
            ref => ref.indexOf(SYNTHETIC_BRANCH_BASE) === 0);

    references = references.map(ref => ref.split(SYNTHETIC_BRANCH_BASE)[1]);

    return new Set(references);
});

/**
 * Delete all redundant synthetic refs within specified 'repo' satisfying
 * `predicate` by recursively iterating over parents of the specified `roots`.
 *
 * Synthetic ref is considered to be redundant if its commit is reachable from
 * descendant who is guaranteed to be around - i.e part of a persistent roots
 * ('roots' here).
 *
 * @param {NodeGit.Repo}   repo
 * @param {Object[]}       roots
 * @param {Function}       predicate
 */
SyntheticGcUtil.prototype.cleanUpRedundant = co.wrap(
    function*(repo, roots, predicate) {

   assert.instanceOf(repo, NodeGit.Repository);

   for (let subPath in roots) {

       const subRepo = yield NodeGit.Repository.open(subPath);

       let existingReferences = yield this.getSyntheticRefs(subRepo);

       for (let subCommit of roots[subPath]) {
           yield this.recursiveSyntheticRefRemoval(subRepo, subCommit,
                                              predicate,
                                              existingReferences);
       }
   }
});

/**
 * Delete all synthetic refs within specified `repo` that satisfy `isOldCommit`,
 * that not part of specified `roots`.
 *
 * @param {NodeGit.Repo}   repo
 * @param {Object[]}       roots
 * @param {Function}       isOldCommit
 */
SyntheticGcUtil.prototype.cleanUpOldRefs = co.wrap(
    function*(repo, roots, isOldCommit) {

   assert.instanceOf(repo, NodeGit.Repository);

   for (let subPath in roots) {

       const subRepo = yield NodeGit.Repository.open(subPath);

       const reservedCommits = roots[subPath];
       const reservedSha = Array.from(reservedCommits).map(function(commit) {
                            return commit.sha();
                        });

       let allRefs = yield this.getSyntheticRefs(subRepo);

       for (let ref of allRefs) {
           // filter out all the references from rootA
           if (reservedSha.includes(ref)) {
               continue;
           }

           const actualCommit = yield subRepo.getCommit(ref);
           if (isOldCommit(actualCommit)) {
               this.removeSyntheticRef(subRepo, actualCommit);
           } // if
       } // for
   } // for
}); // cleanUpOldRefs

/**
* Indicate whether 'ref' is a part of important ref set.
*
* @param {String} ref
* @return {Boolean}
*/
let isImportantRef = function(ref) {

    // `important root` - means the root that most likely be around, so that we
    // can remove all parent synthetic refs.
    const IMPORTANT_REFS = ["refs/heads/", "refs/tags/"];

    for (let importantRef of IMPORTANT_REFS) {
        if (ref.indexOf(importantRef) === 0) {
            return true;
        }
    }

    return false;
};

/**
 * Go through every commit of meta repository and populate a map of submodule
 * to its commits.
 *
 * Return value is a mapping between submodule name and collection of persistent
 * refs within that submodules.
 *
 * @param {NodeGit.Repo}   repo
 * @param {NodeGit.Commit} commit
 * @param {Map<String, Set<String>>} classAroots
 * @return {Map<String, Set<String>>}
 */
SyntheticGcUtil.prototype.populatePerCommit = co.wrap(
    function*(repo, commit, classAroots) {

        const tree = yield commit.getTree();

        const submodules = yield SubmoduleUtil.getSubmoduleNamesForCommit(repo,
                                                                commit);
        for (const subName of submodules) {
            const subRepo =
                yield this.getBareSubmoduleRepo(repo, subName,
                                                commit);
            const subSha = yield tree.entryByPath(subName);
            const subCommit = yield subRepo.getCommit(subSha.sha());
            const subPath = subRepo.path();

            // Record all unique paths from all references.
            if (!(subPath in classAroots)) {
                classAroots[subPath] = new Set();
                this.d_subCommitStored[subPath] = {};
            }

            if (subCommit.sha() in this.d_subCommitStored[subPath]) {
                continue;
            }

            classAroots[subPath].add(subCommit);
            this.d_subCommitStored[subPath][subCommit.sha()] = 1;
        }

        const parents = yield commit.getParents(commit.parentcount());
        let thisState = this;
        yield parents.map(function *(parent) {
            if (parent.sha() in thisState.d_metaVisited) {
                return;
            }
            thisState.d_metaVisited[parent.sha()] = 1;
            classAroots = yield thisState.populatePerCommit(repo,
                                                      parent,
                                                      classAroots);
        });

        return classAroots;
});

/**
 * Fetch all refs that are considered to be persistent within the specified
 * `repo`.
 *
 * Return value is a mapping between submodule name and collection of persistent
 * refs within that submodules.
 *
 * @param {NodeGit.Repo}   repo
 * @return {Map<String, Set<String>>}
 */
SyntheticGcUtil.prototype.populateRoots = co.wrap(
    function*(repo) {

    assert.instanceOf(repo, NodeGit.Repository);

    let classAroots = {}; // roots that we can rely on to be around, master or
                          // team branches

    const refs = yield repo.getReferenceNames(NodeGit.Reference.TYPE.LISTALL);
    for (let ref of refs) {

        if (!isImportantRef(ref)) {
            continue;
        }

        const refHeadCommit = yield repo.getReferenceCommit(ref);
        classAroots = yield this.populatePerCommit(repo,
                                                   refHeadCommit,
                                                   classAroots);
    }

    return classAroots;
});

module.exports = SyntheticGcUtil;

